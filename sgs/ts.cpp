#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iostream>





struct DocumentPtr{

};
class Document {
public:
    // 构造函数
    Document() = default;
    
    // 设置元数据值
    void setMetaValue(const std::string& key, const std::string& value) {
        metadata[key] = value;
    }
    
    // 获取元数据值
    std::string getMetaValue(const std::string& key) const {
        auto it = metadata.find(key);
        if (it != metadata.end()) {
            return it->second;
        }
        return ""; // 如果找不到键，返回空字符串
    }

private:
    std::map<std::string, std::string> metadata; // 用于存储元数据的map
};
struct TrafficRestriction {
    std::string weekday; // 星期
    std::string date; // 日期
    std::string restricted; // 限号

};
struct Rule{
    std::string tab; // 区分一下本地和外地
    std::string tips;
    std::vector<TrafficRestriction> traffic_restriction; //周几，日期，尾号限行

    std::string time; // 限行时间  limitText/time
    std::string area; // 限行范围  limitText/area
    std::string rules; // 限行规则  limitText/rule
};

struct XianxingNode {
    std::string title; // hit/display/title 
    std::string city; // hit/display/city
    std::string startTime; //hit/display/startTime
    std::string endTime; //hit/display/endTime
    std::vector<Rule> rule;
};





XianxingNode da = XianxingNode();
DocumentPtr doc = DocumentPtr();

void xianxing_merge(const XianxingNode& data, DocumentPtr& doc){
    std::stringstream body;

    std::vector<Rule> rules = data.rule;

    body  << data.city << "机动车尾号限行规定" << "\n"
        << "尾号轮巡时间为" << data.startTime << "到"
        << data.endTime << "\n" ;
    
    for (const auto& rule : rules) {
        std::stringstream tmpstr;

        body << rule.tab << "车限号规定如下：" << "\n"
            << rule.tips << "\n";

        for (const auto& restriction : rule.traffic_restriction) {
            body << restriction.weekday << "，日期" 
            << restriction.date << "，尾号限行：" 
            << restriction.restricted << '\n';
        }

        body << "具体说明：" << '\n'
            << "限行时间：" << rule.time << '\n'
            << "限行范围：" << rule.area << '\n'
            << "限行规则：" << rule.rules << '\n';

    }
    std::cout << body.str() <<std::endl;
    // 将生成的字符串设置为doc的body
    //doc.setMetaValue("title", xianxing.title);
    //doc.setMetaValue("body", body.str());
}

struct OilInfo {
    std::string fuel_label; // hit/display/oil_info/fuellabel
    std::string oil_price; // hit/display/oil_info/oilprice
    std::string price_unit; // hit/display/oil_info/price_unit
    std::string direction; // hit/display/oil_info/upanddown
};

struct OilNode {
    std::string city; // hit/city
    std::string date; // hit/date_update
    std::string tips; // hit/tips
    std::vector<OilInfo> oil_info; // 所有油价的vector
};




void oil_merge(const OilNode& data, DocumentPtr& doc){
    std::stringstream body;


    body << data.city << "今日油价如下，油价数据更新日期为：" << data.date << "，数据来源为：" 
        << data.tips << "\n";
    

    for (const auto& info : data.oil_info) {

        body << "燃油标号：" << info.fuel_label 
            << "，最新油价为：" << info.oil_price << info.price_unit 
            << "，较上次调整涨跌为：" << info.direction << '\n';

    }
    std::string restitle = data.city + "油价查询";
    std::cout << body.str() <<std::endl;
    // 将生成的字符串设置为doc的body
    //doc.setMetaValue("title", restitle);
    //doc.setMetaValue("body", body.str());
}
struct GoldItemInfo {
    std::string name; // hit/title/tabList/name
    std::string sub_tab_name; // hit/title/tabList/subTabs/subTabName
    std::string brand; // hit/title/tabList/subTabs/tableData/brand
    std::string productName; // hit/title/tabList/subTabs/tableData/productName
    std::string price; // hit/title/tabList/subTabs/tableData/price
    std::string change; // hit/title/tabList/subTabs/tableData/change
};

struct GoldListNode {
    std::string title; // hit/title
    std::string time; // hit/tabList/time
    std::vector<GoldItemInfo> gold_info; // 所有油价的vector
};




void gold_list_merge(const GoldListNode& data, DocumentPtr& doc){
    std::stringstream body;

    body << data.title << "，数据更新时间" << data.time << "\n";

    for (const auto& info : data.gold_info) {
        body << info.name << "，" << info.sub_tab_name << "：品牌产品名称" << info.brand << info.productName
            << "，价格为：" << info.price << "，涨跌幅为" << info.change << "\n";
    }

    // 将生成的字符串设置为doc的body
    std::cout << body.str() <<std::endl;
    //doc.setMetaValue("title", data.title);
    //doc.setMetaValue("body", body.str());
}






XianxingNode createXianxingNode() {
    XianxingNode node;
    node.title = "北京机动车尾号限行";
    node.city = "北京";
    node.startTime = "07:00";
    node.endTime = "20:00";

    // 初始化 Rule
    Rule rule1;
    rule1.tab = "本地车";
    rule1.tips = "工作日限行";
    rule1.time = "工作日 7:00 - 20:00";
    rule1.area = "五环内";
    rule1.rules = "按尾号限行";

    // 初始化 TrafficRestriction
    TrafficRestriction restriction1;
    restriction1.weekday = "周一";
    restriction1.date = "2025-01-21";
    restriction1.restricted = "1和6";

    TrafficRestriction restriction2;
    restriction2.weekday = "周二";
    restriction2.date = "2025-01-22";
    restriction2.restricted = "2和7";
    rule1.traffic_restriction = std::vector<TrafficRestriction>();
    rule1.traffic_restriction.push_back(restriction1);
    rule1.traffic_restriction.push_back(restriction2);

    node.rule.push_back(rule1);

    return node;
}
OilNode createOilNode() {
    OilNode node;
    node.city = "上海";
    node.date = "2025-01-21";
    node.tips = "中国石油";

    // 初始化 OilInfo
    OilInfo info1;
    info1.fuel_label = "92# 汽油";
    info1.oil_price = "7.50";
    info1.price_unit = "元/升";
    info1.direction = "上涨0.10";

    OilInfo info2;
    info2.fuel_label = "95# 汽油";
    info2.oil_price = "7.90";
    info2.price_unit = "元/升";
    info2.direction = "上涨0.15";

    node.oil_info.push_back(info1);
    node.oil_info.push_back(info2);

    return node;
}
GoldListNode createGoldListNode() {
    GoldListNode node;
    node.title = "今日黄金价格";
    node.time = "2025-01-21 10:00";

    // 初始化 GoldItemInfo
    GoldItemInfo item1;
    item1.name = "黄金";
    item1.sub_tab_name = "AU9999";
    item1.brand = "中国黄金";
    item1.productName = "金条";
    item1.price = "420.00";
    item1.change = "+2.50";

    GoldItemInfo item2;
    item2.name = "白银";
    item2.sub_tab_name = "AG999";
    item2.brand = "中国白银";
    item2.productName = "银条";
    item2.price = "5.50";
    item2.change = "-0.10";

    node.gold_info.push_back(item1);
    node.gold_info.push_back(item2);

    return node;
}


int main() {
    //_setmode(_fileno(stdout), _O_U16TEXT);
    // 初始化 DocumentPtr
    DocumentPtr doc;

    // 创建 XianxingNode 对象并测试
    XianxingNode xianxingNode = createXianxingNode();
    xianxing_merge(xianxingNode, doc);

    // 创建 OilNode 对象并测试
    OilNode oilNode = createOilNode();
    oil_merge(oilNode, doc);

    // 创建 GoldListNode 对象并测试
    GoldListNode goldListNode = createGoldListNode();
    gold_list_merge(goldListNode, doc);

    return 0;
}