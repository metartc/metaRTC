#include <json/json.h>
#include <yangutil/sys/YangJson.h>
#include <yangutil/sys/YangLog.h>
void yang_gen_jsonstr(vector<YangJsonData> &jsons,string &outstr){
	/**
	outstr="{";
	char msg[1024];
	memset(msg,0,sizeof(msg));
	for(size_t i=0;i<jsons.size();i++){
		if(i==0)
			outstr+="\""+jsons[i].key+"\":\""+jsons[i].value+"\"";
		else
			outstr+="\\r\\n,\""+jsons[i].key+"\":\""+jsons[i].value+"\"";
	}
	outstr+="\\r\\n}";**/
		//Json::Value root;
	    Json::FastWriter writer;
	    Json::Value value;

		for(size_t i=0;i<jsons.size();i++){
			value[jsons[i].key]=jsons[i].value;
		}

	    //root.append(value);

		outstr= writer.write(value);
	   // outstr =root.toStyledString();


}


int yang_get_jsonkeystr(string jsonstr,vector<YangJsonData> &jsons){
	Json::Reader reader;
	Json::Value root;
	if(!reader.parse(jsonstr, root)){
		return yang_error_wrap(1,"parse json fail");
	}
	int size = root.size();
	   /** for (int i=0; i<size; ++i)
	    {
	    	jsons.push_back(YangJsonData{root[i].})
	        name = root[i]["name"].asString();
	        age = root[i]["age"].asInt();

	        std::cout<<name<<" "<<age<<std::endl;
	    }**/
	return Yang_Ok;

}
