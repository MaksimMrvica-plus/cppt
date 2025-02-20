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
    // 将生成的字符串设置为doc的body
    doc.setMetaValue("title", xianxing.title);
    doc.setMetaValue("body", body.str());
}


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
    // 将生成的字符串设置为doc的body
    doc.setMetaValue("title", restitle);
    doc.setMetaValue("body", body.str());
}


void gold_list_merge(const GoldListNode& data, DocumentPtr& doc){
    std::stringstream body;

    body << data.title << "，数据更新时间" << data.time << "\n";

    for (const auto& info : data.gold_info) {
        body << info.name << "，" << info.sub_tab_name << "：品牌产品名称" << info.brand << info.productName
            << "，价格为：" << info.price << "，涨跌幅为" << info.change << "\n";
    }

    // 将生成的字符串设置为doc的body
    doc.setMetaValue("title", data.title);
    doc.setMetaValue("body", body.str());
}



