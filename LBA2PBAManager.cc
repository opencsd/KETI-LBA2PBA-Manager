#include "LBA2PBAManager.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

using StorageEngineInstance::LBA2PBAManager;
using StorageEngineInstance::LBARequest;
using StorageEngineInstance::PBAResponse;
using StorageEngineInstance::PBAList;
using StorageEngineInstance::PBA;
using StorageEngineInstance::LBA;
using StorageEngineInstance::Chunk;

PBAResponse RunLBA2PBA(LBARequest request){
	PBAResponse response;

	off64_t offset_buffer[128][3];
	// string ordering = request.ordering();

	for(int i=0; i<request.file_lba_list_size(); i++){
		PBAList pba_list;
	
		string file_name = request.file_lba_list(i).file_name();
		KETILOG("LBA2PBA Manager","File Name: "+file_name);

		pba_list.set_file_name(file_name);
		
		//do hdparm
		char cmd[256];
		string fdName = csdmap[file_name]; //원래는 리스트 -> 지금은 테스트로 csd 하나만

		sprintf(cmd,"filefrag -e /mnt/%s/sst/%s 2> /dev/null",fdName.c_str(),file_name.c_str());
		cout << cmd << endl;//file frag 실행
		string csdID = fdName.substr(8,1).c_str();//new_port1 -> 1
		KETILOG("LBA2PBA Manager","CSD ID: "+csdID);
		
		char buf[MAXLINE];
		int flag = 0;
		FILE *fp=popen(cmd,"r");
		int index = 0;
		while(fgets(buf, MAXLINE, fp) != NULL){
        	std::string line(buf);
        	// tokenize
        	auto token = split(line,' ');
        	// trim
        	for(int i=0;i<token.size();i++){
	            token[i] = trim(token[i],".:\n\r");
        	}
        
        	if(flag){
	            // for(auto e:token){
                // 	printf("%s\n",e.c_str());
            	// }
            	offset_buffer[index][0] = (off64_t)4096 * atoi(token[1].c_str()); //lba offset start
            	offset_buffer[index][1] = (off64_t)4096 * atoi(token[3].c_str()); //pba offset start
            	offset_buffer[index][2] = (off64_t)4096 * atoi(token[5].c_str()); //block length
				index++;
        	}

			// start check
        	if(token[0] == "ext"){
	            flag = 1;
        	}

        	// last check
        	for(auto e:token){
	            if(e=="last,eof"){
                	// end
                	// printf("end\n");
                	flag = 0;
            	}
        	}
		}
		int tbl_size = index;
		pclose(fp);
		// for(int k = 0; k < index; k++){
		// 	cout << offset_buffer[k][0] << " " << offset_buffer[k][1] << " " << offset_buffer[k][2] << endl;
		// }

		off64_t req_offset;
		off64_t req_length;

		PBA pba;
		pba.set_csd_id(csdID);

		for(int j=0;j<request.file_lba_list(i).chunks_size();j++){
			Chunk lba_chunk = request.file_lba_list(i).chunks(j);
			 
			// std::cout << "Offset : " << lba_chunk.offset() << std::endl;
			// std::cout << "Length : " << lba_chunk.length() << std::endl;

			flag = 0;
			req_offset = lba_chunk.offset();
			req_length = lba_chunk.length();

			for(int k = 0; k < tbl_size; k++){
				Chunk pba_chunk;
				if(flag || req_offset >= offset_buffer[k][0] && req_offset < offset_buffer[k][0] + offset_buffer[k][2]){
					flag = 1;
					if(req_length > offset_buffer[k][2]){ // X
						// printf("{\n\t\"Offset\" : %ld,\n\t\"Length\" : %ld\n},\n",offset_buffer[k][1] + req_offset - offset_buffer[k][0],offset_buffer[k][2]);
						pba_chunk.set_offset(offset_buffer[k][1] + req_offset - offset_buffer[k][0]);
						pba_chunk.set_length(offset_buffer[k][2]);
						pba.add_chunks()->CopyFrom(pba_chunk); //push back res offset to offset list

						req_length -= offset_buffer[k][2];
						req_offset += offset_buffer[k][2];
					} else { // here
						// printf("{\n\t\"Offset\" : %ld,\n\t\"Length\" : %ld\n},\n",offset_buffer[k][1] + req_offset - offset_buffer[k][0],req_length);
						pba_chunk.set_offset(offset_buffer[k][1] + req_offset - offset_buffer[k][0]);
						pba_chunk.set_length(req_length);
						pba.add_chunks()->CopyFrom(pba_chunk); //push back res offset to offset list
						break;
					}
				}
			}
		}



		pba_list.add_csd_pba_list()->CopyFrom(pba);
		response.add_file_csd_list()->CopyFrom(pba_list);
	}
	
	return response;
}

class LBA2PBAManagerServiceImpl final : public LBA2PBAManager::Service {
  Status RequestPBA(ServerContext* context, const LBARequest* request, PBAResponse* response) override {
    KETILOG("LBA2PBA Manager", "==:Receive Snippet from Interface Container:==");
	
	{
	// Check LBA Request - Debug Code   
	std::string test_json;
	google::protobuf::util::JsonPrintOptions options;
	options.always_print_primitive_fields = true;
	options.always_print_enums_as_ints = true;
	google::protobuf::util::MessageToJsonString(*request,&test_json,options);
	std::cout << "LBA Request" << std::endl;
	// std::cout << test_json << std::endl << std::endl;
	}

	PBAResponse res;
	res = RunLBA2PBA(*request);
	response->CopyFrom(res);

	{
	// Check PBA Response - Debug Code   
	std::string test_json;
	google::protobuf::util::JsonPrintOptions options;
	options.always_print_primitive_fields = true;
	options.always_print_enums_as_ints = true;
	google::protobuf::util::MessageToJsonString(*response,&test_json,options);
	std::cout << "PBA Response" << std::endl;
	// std::cout << test_json << std::endl << std::endl;
	}

    return Status::OK;
  }
};

void RunServer() {
  std::string server_address((std::string)STORAGE_NODE_IP+":"+std::to_string(LBA2PBA_MANAGER_PORT));
  LBA2PBAManagerServiceImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());

  KETILOG("LBA2PBA Manager", "LBA2PBA Manager Server listening on "+server_address);

  server->Wait();
}

int main(int argc, char* argv[]){	
	InitLBA2PBAManager();
	RunServer();
	return 0;
}