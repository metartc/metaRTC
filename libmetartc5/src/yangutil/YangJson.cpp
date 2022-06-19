
#include <yangutil/sys/YangJson.h>
#include <yangutil/sys/YangLog.h>
#ifndef __ANDROID__
#include <json/json.h>
void yang_gen_jsonstr(vector<YangJsonData> &jsons,string &outstr){

		//Json::Value root;
	    Json::FastWriter writer;
	    Json::Value value;

		for(size_t i=0;i<jsons.size();i++){
			value[jsons[i].key]=jsons[i].value;
		}

	    //root.append(value);

		outstr= writer.write(value);



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
#endif
