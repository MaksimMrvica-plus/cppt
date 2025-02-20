curl --location 'http://33.52.1.246:37445/execute?session_id=12345' \
--header 'Content-Type: application/json; charset=utf-8' \
--data '
import json

def search(query_list, tag="general"):
    main_query_rw = query_list[0]
    if isinstance(main_query_rw, str):
        main_query = main_query_rw
        main_query_rw_type = "none"
    else:
        main_query = main_query_rw[0]
        main_query_rw_type = main_query_rw[1]
    
    sub_query_list = []
    for query in query_list[1:]:
        sub_query_list.append([query[0], query[1]])

    result = {
        "tool_name": "search",
        "main_query": [main_query, main_query_rw_type],
        "sub_queries": sub_query_list,
        "tag": tag
    }

    return json.dumps(result, ensure_ascii=False)

output = search([("阿里巴巴公司概况", "parallel"), ("阿里巴巴公司介绍", "normalize")])
print(output)
'