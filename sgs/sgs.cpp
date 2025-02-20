#include <string>


struct ScNode {
  std::string scName;
  // std::string modifyQ;
  KeyValueMap params;
};

bool ArbiterWrapper::modifyWeatherQ(const KeyValueMap& params,
                                    const std::string& scName,
                                    std::vector<ScNode>& scNodes) {
  ScNode sc;
  sc.scName = scName;
  sc.params = params;
  auto it = params.find(WEATHER_TOOL_CITY);
  if (it == params.end()) {
    LANGFRAME_LOG(ERROR, "[%s], weather city is null",
                  commonArgs().reqid.c_str());
    return false;
  }
  sc.params["q"] = it->second + "天气";
  sc.params["normal_request_sc"] = "weather_moji";
  scNodes.emplace_back(sc);
  return true;
}

bool ArbiterWrapper::modifyStock(const KeyValueMap& params,
                                 const std::string& scName,
                                 std::vector<ScNode>& scNodes) {
  std::vector<std::string> stockList{"港股", "美股", "A股"};
  for (const std::string& stock : stockList) {
    ScNode sc;
    sc.scName = scName;
    sc.params = params;
    auto it = params.find(STOCK_TOOL_STOCK_NAME);
    if (it == params.end()) {
      LANGFRAME_LOG(ERROR, "[%s], stock name is null",
                    commonArgs().reqid.c_str());
      return false;
    }
    std::string stockName = it->second;
    sc.params["q"] = stockName + stock;
    sc.params["normal_request_sc"] = "finance_stock_new";
    scNodes.emplace_back(sc);
  }

  return true;
}


bool ArbiterWrapper::modifyExchangeRate(const KeyValueMap& params,
                                    const std::string& scName,
                                    std::vector<ScNode>& scNodes) {
  ScNode sc;
  sc.scName = scName;
  sc.params = params;
  auto it = params.find(EXCHANGE_TOOL_FROM);
  if (it == params.end()) {
    LANGFRAME_LOG(ERROR, "[%s], currency is null",
                  commonArgs().reqid.c_str());
    return false;
  }
  sc.params["q"] = "汇率";
  sc.params["normal_request_sc"] = "exchange_rate";
  scNodes.emplace_back(sc);
  return true;
}


bool ArbiterWrapper::modifyCustomerPhone(const KeyValueMap& params,
                                    const std::string& scName,
                                    std::vector<ScNode>& scNodes) {
  ScNode sc;
  sc.scName = scName;
  sc.params = params;
  auto it = params.find(PHONE_TOOL_ENTITY);
  if (it == params.end()) {
    LANGFRAME_LOG(ERROR, "[%s], institution is null",
                  commonArgs().reqid.c_str());
    return false;
  }
  std::string institution = it->second;
  sc.params["q"] = institution;
  sc.params["normal_request_sc"] = "customer_phone";
  scNodes.emplace_back(sc);
  return true;
}


bool ArbiterWrapper::modifyCalculate(const KeyValueMap& params,
                                    const std::string& scName,
                                    std::vector<ScNode>& scNodes) {
  ScNode sc;
  sc.scName = scName;
  sc.params = params;
  auto it = params.find(PARAM_INPUT_KEY);
  if (it == params.end()) {
    LANGFRAME_LOG(ERROR, "[%s], calculation is null",
                  commonArgs().reqid.c_str());
    return false;
  }
  std::string output = it->second;
  std::string result = R"("code":0,"error":"OK","output":")" + output + R"(")";
  sc.params["q"] = result;
  sc.params["normal_request_sc"] = "result";
  scNodes.emplace_back(sc);
  return true;
}

bool ArbiterWrapper::modifyNews(const KeyValueMap& params,
                                    const std::string& scName,
                                    std::vector<ScNode>& scNodes) {
  return true;
}




// sgs 

bool ArbiterWrapper::modifyFund(const KeyValueMap& params,
                                    const std::string& scName,
                                    std::vector<ScNode>& scNodes) {
  ScNode sc;
  sc.scName = scName;
  sc.params = params;
  auto it = params.find(FUND_TOOL_FUND_NAME);
  if (it == params.end()) {
    LANGFRAME_LOG(ERROR, "[%s], fund name is null",
                  commonArgs().reqid.c_str());
    return false;
  }
  std::string fundName = it->second;
  sc.params["q"] = fundName;
  sc.params["request_sc"] = "shortcut_searcher::fund_sc";
  scNodes.emplace_back(sc);
  return true;
}


bool ArbiterWrapper::modifyMetal(const KeyValueMap& params,
                                    const std::string& scName,
                                    std::vector<ScNode>& scNodes) {
  std::vector<std::string> metalList{"shortcut-searcher-3::gold_trendchart", "shortcut-searcher-3::gold_list"};
  for (const std::string& metal : metalList) {
    ScNode sc;
    sc.scName = scName;
    sc.params = params;
    // 贵金属的名称
    auto itName = params.find(METAL_TOOL_METAL_NAME);
    if (itName == params.end()) {
      LANGFRAME_LOG(ERROR, "[%s], metal name is null",
                    commonArgs().reqid.c_str());
      return false;
    }
    std::string metalName = itName->second;
    // 贵金属商品类型：期货 / 现货
    auto itType = params.find(METAL_TOOL_GOOD_TYPE);
    if (itType == params.end()) {
      LANGFRAME_LOG(ERROR, "[%s], metal type is null",
                    commonArgs().reqid.c_str());
      return false;
    }
    std::string metalType = itType->second;

    sc.params["q"] = metalType + metalName;
    sc.params["request_sc"] = metal;
    scNodes.emplace_back(sc);
  }
  return true;
}


bool ArbiterWrapper::modifyTime(const KeyValueMap& params,
                                    const std::string& scName,
                                    std::vector<ScNode>& scNodes) {
  ScNode sc;
  sc.scName = scName;
  sc.params = params;
  auto it = params.find(TIME_TOOL_TIMEZONE);
  if (it == params.end()) {
    LANGFRAME_LOG(ERROR, "[%s], timezone is null",
                  commonArgs().reqid.c_str());
    return false;
  }
  std::string timeZone = it->second;
  sc.params["q"] = timeZone + "时间";
  sc.params["request_sc"] = "shortcut-searcher-6::time_sc";
  scNodes.emplace_back(sc);
  return true;
}



bool ArbiterWrapper::modifyOilPrice(const KeyValueMap& params,
                                    const std::string& scName,
                                    std::vector<ScNode>& scNodes) {
  ScNode sc;
  sc.scName = scName;
  sc.params = params;
  auto it = params.find(OIL_TOOL_LOC);
  if (it == params.end()) {
    LANGFRAME_LOG(ERROR, "[%s], oil location is null",
                  commonArgs().reqid.c_str());
    return false;
  }
  std::string oilLoc = it->second;
  sc.params["q"] = oilLoc + "油价";
  sc.params["request_sc"] = "shortcut-searcher-6::oil_price";
  scNodes.emplace_back(sc);
  return true;
}


// 将日期从 YYYY-MM-DD 格式转换为 YYYY年M月D日 格式
std::string convertDate(const std::string& calendarDate) {
    std::istringstream ss(calendarDate);
    int year, month, day;
    char dash1, dash2; // 用于存储分隔符'-'

    // 按格式提取年、月、日
    if (ss >> year >> dash1 >> month >> dash2 >> day && dash1 == '-' && dash2 == '-') {
        // 使用ostringstream格式化日期
        std::ostringstream oss;
        oss << year << "年" << month << "月" << day << "日";
        return oss.str();
    } else {
        // 如果输入格式错误，返回空字符串或抛出异常
        return ""; // 或者可以抛出异常 std::invalid_argument("日期格式错误")
    }
}

bool ArbiterWrapper::modifyCalendar(const KeyValueMap& params,
                                    const std::string& scName,
                                    std::vector<ScNode>& scNodes) {
  ScNode sc;
  sc.scName = scName;
  sc.params = params;
  // 日期
  auto itDate = params.find(CALENDAR_TOOL_DATE);
  if (itDate == params.end()) {
    LANGFRAME_LOG(ERROR, "[%s], calendar date is null",
                  commonArgs().reqid.c_str());
    return false;
  }
  std::string calendarDate = convertDate(itDate->second);
  // 类型：阳历 / 阴历
  auto itType = params.find(CALENDAR_TOOL_TYPE);
  if (itType == params.end()) {
    LANGFRAME_LOG(ERROR, "[%s], calendar type is null",
                  commonArgs().reqid.c_str());
    return false;
  }
  std::string calendarType = itType->second;

  sc.params["q"] = calendarType + calendarDate;
  sc.params["request_sc"] = "shortcur-searcher-4::rili";
  scNodes.emplace_back(sc);
  return true;
}


bool ArbiterWrapper::modifyFestival(const KeyValueMap& params,
                                    const std::string& scName,
                                    std::vector<ScNode>& scNodes) {
  ScNode sc;
  sc.scName = scName;
  sc.params = params;
  // 节日名称
  auto itName = params.find(FESTIVAL_TOOL_FEST_NAME);
  if (itName == params.end()) {
    LANGFRAME_LOG(ERROR, "[%s], festival name is null",
                  commonArgs().reqid.c_str());
    return false;
  }
  std::string festivalName = itName->second;

  // 年份
  auto itYear = params.find(FESTIVAL_TOOL_YEAR);
  if (itYear == params.end()) {
    LANGFRAME_LOG(ERROR, "[%s], festival year is null",
                  commonArgs().reqid.c_str());
    // return false;
    std::string festivalYear = "";
  }
  else:
    std::string festivalYear = itYear->second;

  sc.params["q"] = festivalYear + festivalName;
  sc.params["request_sc"] = "shortcur-searcher-4::rili";
  scNodes.emplace_back(sc);
  return true;
}


bool ArbiterWrapper::modifyRestriction(const KeyValueMap& params,
                                    const std::string& scName,
                                    std::vector<ScNode>& scNodes) {
  ScNode sc;
  sc.scName = scName;
  sc.params = params;
  auto it = params.find(RESTRICTION_TOOL_LOC);
  if (it == params.end()) {
    LANGFRAME_LOG(ERROR, "[%s], restriction city is null",
                  commonArgs().reqid.c_str());
    return false;
  }
  std::string restrictionCity = it->second;
  sc.params["q"] = restrictionCity + "机动车限行";
  sc.params["request_sc"] = "request_sc=shortcur-searcher-4::xianxing";
  scNodes.emplace_back(sc);
  return true;
}
