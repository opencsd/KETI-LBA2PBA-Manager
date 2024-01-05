#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unordered_map>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" 

#include <grpcpp/grpcpp.h>
#include "lba2pba.grpc.pb.h"

using namespace rapidjson;
using namespace std;

#define STORAGE_NODE_IP "10.0.4.83"
#define LBA2PBA_MANAGER_PORT 40301
#define MAXLINE 256
#define BUFF_SIZE 4096
#define CSDID 1
#define LOGTAG "LBA2PBA Manager"

unordered_map<string,string> csdmap;//key:filename, value:csdid

void InitLBA2PBAManager(){
	/* ==============================================*/
	/* ================= tpch small =================*/
	/* ==============================================*/

	//--supplier--
	csdmap.insert(make_pair("002189.sst","newport_1")); 
	csdmap.insert(make_pair("002191.sst","newport_2")); 
	csdmap.insert(make_pair("002250.sst","newport_3")); 
	csdmap.insert(make_pair("002251.sst","newport_4")); 
	csdmap.insert(make_pair("002252.sst","newport_5"));
	csdmap.insert(make_pair("002253.sst","newport_6"));
	csdmap.insert(make_pair("002312.sst","newport_7"));
	csdmap.insert(make_pair("002190.sst","newport_8"));

	//--region--
	csdmap.insert(make_pair("002005.sst","newport_1"));

	//--customer--
	csdmap.insert(make_pair("002065.sst","newport_1"));
	csdmap.insert(make_pair("002066.sst","newport_2"));
	csdmap.insert(make_pair("002067.sst","newport_3"));
	csdmap.insert(make_pair("002126.sst","newport_4"));
	csdmap.insert(make_pair("002127.sst","newport_5"));
	csdmap.insert(make_pair("002128.sst","newport_6"));
	csdmap.insert(make_pair("002129.sst","newport_7"));
	csdmap.insert(make_pair("002188.sst","newport_8"));

	//--lineitem--
	csdmap.insert(make_pair("001509.sst","newport_1"));
	csdmap.insert(make_pair("001568.sst","newport_2"));
	csdmap.insert(make_pair("001569.sst","newport_3"));
	csdmap.insert(make_pair("001570.sst","newport_4"));
	csdmap.insert(make_pair("001571.sst","newport_5"));
	csdmap.insert(make_pair("001630.sst","newport_6"));
	csdmap.insert(make_pair("001631.sst","newport_7"));
	csdmap.insert(make_pair("001632.sst","newport_8"));

	//--nation--
	csdmap.insert(make_pair("002064.sst","newport_2"));

	//--orders--
	csdmap.insert(make_pair("001633.sst","newport_1"));
	csdmap.insert(make_pair("001692.sst","newport_2"));
	csdmap.insert(make_pair("001693.sst","newport_3"));
	csdmap.insert(make_pair("001694.sst","newport_4"));
	csdmap.insert(make_pair("001695.sst","newport_5"));
	csdmap.insert(make_pair("001754.sst","newport_6"));
	csdmap.insert(make_pair("001755.sst","newport_7"));
	csdmap.insert(make_pair("001756.sst","newport_8"));

	//--part--
	csdmap.insert(make_pair("001757.sst","newport_1"));
	csdmap.insert(make_pair("001816.sst","newport_2"));
	csdmap.insert(make_pair("001817.sst","newport_3"));
	csdmap.insert(make_pair("001818.sst","newport_4"));
	csdmap.insert(make_pair("001819.sst","newport_5"));
	csdmap.insert(make_pair("001878.sst","newport_6"));
	csdmap.insert(make_pair("001879.sst","newport_7"));
	csdmap.insert(make_pair("001880.sst","newport_8"));

	//--partsupp--
	csdmap.insert(make_pair("001881.sst","newport_1"));
	csdmap.insert(make_pair("001940.sst","newport_2"));
	csdmap.insert(make_pair("001941.sst","newport_3"));
	csdmap.insert(make_pair("001942.sst","newport_4"));
	csdmap.insert(make_pair("001943.sst","newport_5"));
	csdmap.insert(make_pair("002002.sst","newport_6"));
	csdmap.insert(make_pair("002003.sst","newport_7"));
	csdmap.insert(make_pair("002004.sst","newport_8"));




	/* ==============================================*/
	/* ================= tpch origin ================*/
	/* ==============================================*/

	//--supplier--
	csdmap.insert(make_pair("000788.sst","newport_1")); 
	csdmap.insert(make_pair("000847.sst","newport_2")); 
	csdmap.insert(make_pair("000848.sst","newport_3")); 
	csdmap.insert(make_pair("000849.sst","newport_4")); 
	csdmap.insert(make_pair("000850.sst","newport_5"));
	csdmap.insert(make_pair("000923.sst","newport_6"));
	csdmap.insert(make_pair("000924.sst","newport_7"));
	csdmap.insert(make_pair("000925.sst","newport_8"));

	//--region--
	//csdmap.insert(make_pair("000662.sst","newport_1"));
	csdmap.insert(make_pair("002005.sst","newport_1"));

	//--customer--
	csdmap.insert(make_pair("000664.sst","newport_1"));
	csdmap.insert(make_pair("000723.sst","newport_2"));
	csdmap.insert(make_pair("000724.sst","newport_3"));
	csdmap.insert(make_pair("000725.sst","newport_4"));
	csdmap.insert(make_pair("000726.sst","newport_5"));
	csdmap.insert(make_pair("000785.sst","newport_6"));
	csdmap.insert(make_pair("000786.sst","newport_7"));
	csdmap.insert(make_pair("000787.sst","newport_8"));

	//--lineitem--
	csdmap.insert(make_pair("001379.sst","newport_1"));
	csdmap.insert(make_pair("001383.sst","newport_2"));
	csdmap.insert(make_pair("001400.sst","newport_3"));
	csdmap.insert(make_pair("001435.sst","newport_4"));
	csdmap.insert(make_pair("001437.sst","newport_5"));
	csdmap.insert(make_pair("001472.sst","newport_6"));
	csdmap.insert(make_pair("001474.sst","newport_7"));
	csdmap.insert(make_pair("001506.sst","newport_8"));

	csdmap.insert(make_pair("001382.sst","newport_1"));
	csdmap.insert(make_pair("001384.sst","newport_2"));
	csdmap.insert(make_pair("001434.sst","newport_3"));
	csdmap.insert(make_pair("001436.sst","newport_4"));
	csdmap.insert(make_pair("001508.sst","newport_5"));
	csdmap.insert(make_pair("001471.sst","newport_6"));
	csdmap.insert(make_pair("001473.sst","newport_7"));
	csdmap.insert(make_pair("001507.sst","newport_8"));

	//--nation--
	//csdmap.insert(make_pair("000663.sst","newport_2"));
	csdmap.insert(make_pair("002064.sst","newport_2"));

	//--orders--
	csdmap.insert(make_pair("000290.sst","newport_1"));
	csdmap.insert(make_pair("000291.sst","newport_2"));
	csdmap.insert(make_pair("000292.sst","newport_3"));
	csdmap.insert(make_pair("000351.sst","newport_4"));
	csdmap.insert(make_pair("000352.sst","newport_5"));
	csdmap.insert(make_pair("000353.sst","newport_6"));
	csdmap.insert(make_pair("000354.sst","newport_7"));
	csdmap.insert(make_pair("000413.sst","newport_8"));

	//--part--
	csdmap.insert(make_pair("003805.sst","newport_1"));
	csdmap.insert(make_pair("003806.sst","newport_2"));
	csdmap.insert(make_pair("003807.sst","newport_3"));
	csdmap.insert(make_pair("003808.sst","newport_4"));
	csdmap.insert(make_pair("003809.sst","newport_5"));
	csdmap.insert(make_pair("003810.sst","newport_6"));
	csdmap.insert(make_pair("003811.sst","newport_7"));
	csdmap.insert(make_pair("003812.sst","newport_8"));

	//--partsupp--
	csdmap.insert(make_pair("000538.sst","newport_1"));
	csdmap.insert(make_pair("000539.sst","newport_2"));
	csdmap.insert(make_pair("000540.sst","newport_3"));
	csdmap.insert(make_pair("000599.sst","newport_4"));
	csdmap.insert(make_pair("000600.sst","newport_5"));
	csdmap.insert(make_pair("000601.sst","newport_6"));
	csdmap.insert(make_pair("000602.sst","newport_7"));
	csdmap.insert(make_pair("000661.sst","newport_8"));




	/* ==============================================*/
	/* ============== tpch 1m no index ==============*/
	/* ==============================================*/

	//--supplier--
	csdmap.insert(make_pair("003153.sst","newport_1")); 
	csdmap.insert(make_pair("003154.sst","newport_2")); 
	csdmap.insert(make_pair("003213.sst","newport_3")); 
	csdmap.insert(make_pair("003214.sst","newport_4")); 
	csdmap.insert(make_pair("003215.sst","newport_5"));
	csdmap.insert(make_pair("003216.sst","newport_6"));
	csdmap.insert(make_pair("003287.sst","newport_7"));
	csdmap.insert(make_pair("003288.sst","newport_8"));

	//--region--
	//csdmap.insert(make_pair("003027.sst","newport_1"));
	csdmap.insert(make_pair("002005.sst","newport_1"));

	//--customer--
	csdmap.insert(make_pair("003289.sst","newport_1"));
	csdmap.insert(make_pair("003290.sst","newport_2"));
	csdmap.insert(make_pair("003349.sst","newport_3"));
	csdmap.insert(make_pair("003350.sst","newport_4"));
	csdmap.insert(make_pair("003351.sst","newport_5"));
	csdmap.insert(make_pair("003352.sst","newport_6"));
	csdmap.insert(make_pair("003354.sst","newport_7"));
	csdmap.insert(make_pair("003394.sst","newport_8"));

	//--lineitem--
	csdmap.insert(make_pair("002531.sst","newport_1"));
	csdmap.insert(make_pair("002532.sst","newport_2"));
	csdmap.insert(make_pair("002533.sst","newport_3"));
	csdmap.insert(make_pair("002534.sst","newport_4"));
	csdmap.insert(make_pair("002593.sst","newport_5"));
	csdmap.insert(make_pair("002594.sst","newport_6"));
	csdmap.insert(make_pair("002595.sst","newport_7"));
	csdmap.insert(make_pair("002596.sst","newport_8"));

	//--nation--
	//csdmap.insert(make_pair("003028.sst","newport_2"));
	csdmap.insert(make_pair("002064.sst","newport_2"));

	//--orders--
	csdmap.insert(make_pair("002655.sst","newport_1"));
	csdmap.insert(make_pair("002656.sst","newport_2"));
	csdmap.insert(make_pair("002657.sst","newport_3"));
	csdmap.insert(make_pair("002658.sst","newport_4"));
	csdmap.insert(make_pair("002717.sst","newport_5"));
	csdmap.insert(make_pair("002718.sst","newport_6"));
	csdmap.insert(make_pair("002719.sst","newport_7"));
	csdmap.insert(make_pair("002720.sst","newport_8"));

	//--part--
	csdmap.insert(make_pair("002779.sst","newport_1"));
	csdmap.insert(make_pair("002780.sst","newport_2"));
	csdmap.insert(make_pair("002781.sst","newport_3"));
	csdmap.insert(make_pair("002782.sst","newport_4"));
	csdmap.insert(make_pair("002841.sst","newport_5"));
	csdmap.insert(make_pair("002842.sst","newport_6"));
	csdmap.insert(make_pair("002843.sst","newport_7"));
	csdmap.insert(make_pair("002844.sst","newport_8"));

	//--partsupp--
	csdmap.insert(make_pair("002903.sst","newport_1"));
	csdmap.insert(make_pair("002904.sst","newport_2"));
	csdmap.insert(make_pair("002905.sst","newport_3"));
	csdmap.insert(make_pair("002906.sst","newport_4"));
	csdmap.insert(make_pair("002965.sst","newport_5"));
	csdmap.insert(make_pair("002966.sst","newport_6"));
	csdmap.insert(make_pair("002967.sst","newport_7"));
	csdmap.insert(make_pair("002968.sst","newport_8"));


}

// trim left 
inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v"){
	s.erase(0, s.find_first_not_of(t));
	return s;
}
// trim right 
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v"){
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}
// trim left & right 
inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v"){
	return ltrim(rtrim(s, t), t);
}

std::vector<std::string> split(std::string str, char delimiter){
    std::vector<std::string> answer;
    std::stringstream ss(str);
    std::string temp;
 
    while (getline(ss, temp, delimiter)) {
        if(temp == "" || temp == "\n" || temp == "\r\n"){
            continue;
        }
        answer.push_back(temp);
    }

    return answer;
}


inline void KETILOG(std::string id, std::string msg){
    std::cout << "[" << id << "] " << msg << std::endl;
}