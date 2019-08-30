#include "httplib.h"
#include "db.hpp"
#include <signal.h>
MYSQL* mysql = NULL;
int main()
{
    using namespace blog_system;
    using namespace httplib;
    // 1. 先和数据库建立好连接
    mysql = blog_system::MySQLInit();
    signal(SIGINT, [](int signo){blog_system::MySQLRelease(mysql);exit(0);});

    // 2. 创建相关数据库处理对象
    blog_system::BlogTable blog_table(mysql);
    blog_system::TagTable tag_table(mysql);
    // 3. 创建服务器，并设置“路由“ (HTTP服务器中的路由)
    // 把方法+path 映射到哪个处理函数 关联关系声明清楚
    Server server;
    // 新增博客
    server.Post("/blog", [&blog_table](const Request& req, Response& resp)
                {
                printf("新增博客！\n");
                // 1. 获取到请求中的body并解析成json
                Json::Reader reader; // 字符串转Json对象
                Json::FastWriter writer; 
                Json::Value req_json; // json格式请求
                Json::Value resp_json;
                bool ret = reader.parse(req.body, req_json); // reader.parse() req.body:解析的字符串  req_json保存输出结果
                if(!ret)  
                {
                    // 解析出错，
                    printf("解析请求失败！%s\n", req.body.c_str());
                    // 构响应对象，告诉客户端出错
                    resp_json["ok"] = false;
                    resp_json["reason"] = "input data parse fail!";
                    resp.status = 400;
                    resp.set_content(writer.write(resp_json), "application/json"); //结果返回给客户端 
                    return;
                }
                // 2. 对参数进行校验 
                if(req_json["title"].empty() || req_json["content"].empty()
                   || req_json["tag_id"].empty() || req_json["create_time"].empty())
                {
                    printf("请求数据格式出错！%s\n",req.body.c_str());
                    resp_json["ok"] = false;
                    resp_json["reason"] = "input data format error!";
                    resp.status = 400;
                    resp.set_content(writer.write(resp_json), "application/json");
                    
                    return;
                }

                // 3. 调用mysql接口操作
                ret = blog_table.Insert(req_json);
                if(!ret)
                {
                    printf("博客插入失败！");
                    resp_json["ok"] = false;
                    resp_json["reason"] = "blog insert failed!";
                    resp.status = 500;
                    resp.set_content(writer.write(resp_json), "application/json");
                }

                // 4. 调用正确响应给客户端
                printf("博客插入成功！\n");
                resp_json["ok"] = true;
                resp.set_content(writer.write(resp_json), "application/json");
                return ;
                });
    
    //查看所有博客列表 
    server.Get("/blog", [&blog_table](const Request& req, Response& resp)
               {
               printf("查看所有博客！\n");
               Json::FastWriter writer;
               // 1. 获取tag_id，如果tag_id不存在 返回空字符串
               const std::string& tag_id = req.get_param_value("tag_id");
               // 此处无需解析请求，也无需合法性判断
               // 2.调用数据库操作获取博客结果
               Json::Value resp_json;
               bool ret = blog_table.SelectAll(&resp_json, tag_id);
               if(!ret)
               {
                   resp_json["ok"] = false;
                   resp_json["reason"] = "select all failed";
                   resp.status = 500;
                   resp.set_content(writer.write(resp_json), "application/json");
                   return;
               }
               // 3. 成功，构造响应结果
               resp.set_content(writer.write(resp_json), "application/json");
               return;
               });
    
    // 查看某个博客
    server.Get(R"(/blog/(\d+))", [&blog_table](const Request& req,Response& resp)
               {
                   Json::FastWriter writer;
                   // 1. 解析获取blog_id
                   int32_t blog_id = std::stoi(req.matches[1].str());
                   printf("查看id为 %d 的博客！\n", blog_id);
                   // 2. 直接调用数据库操作
                   Json::Value resp_json;
                   bool ret = blog_table.SelectOne(blog_id, &resp_json);
                   if(!ret)
                   {
                       resp_json["ok"] = false;
                       resp_json["reason"] = "查看该博客失败: " + std::to_string(blog_id);
                       resp.status = 404;
                       resp.set_content(writer.write(resp_json), "application/json");
                       return;
                   }
                   // 3. 包装一个执行正确的响应
                   resp.set_content(writer.write(resp_json), "application/json");
                   return;
               });
    
    //修改某个博客
    server.Put(R"(/blog/(\d+))",[&blog_table](const Request& req, Response& resp)
               {
               // 1. 获取博客id
               int32_t blog_id = std::stoi(req.matches[1].str());
               printf("修改id为 %d 的博客！\n", blog_id);
               // 2. 获取请求结果并解析结果
               Json::Reader reader;
               Json::FastWriter writer;
               Json::Value req_json;
               Json::Value resp_json;

               bool ret = reader.parse(req.body, req_json);
               if(!ret)
               {
               resp_json["ok"] = false;
               resp_json["reason"] = "updata blog parse request failed";
               resp.status = 400;
               resp.set_content(writer.write(resp_json), "application/json");
               return;
               }

               // 3. 校验参数是否正确
               if(req_json["title"].empty() || req_json["content"].empty()
                  || req_json["tag_id"].empty())
               {
                   resp_json["ok"] = false;
                   resp_json["reason"] = "updata blog request format error";
                   resp.status = 400;
                   resp.set_content(writer.write(resp_json), "application/json");
                   return;
               }
               
               // 4.调用数据库操作更新博客
               req_json["blog_id"] = blog_id; // 从path中获取id 设置到json对象中             
               ret = blog_table.Update(req_json);
               if(!ret)
               {
                   resp_json["ok"] = false;
                   resp_json["reason"] = "updata blog database faile";
                   resp.status = 500;
                   resp.set_content(writer.write(resp_json), "application/json");
                   return;
               }
               // 5. 构造一个正确的返回结果
               resp_json["ok"] = true;
               resp.set_content(writer.write(resp_json), "application/json");
               return;
               });
    
    // 删除
    server.Delete(R"(/blog/(\d+))", [&blog_table](const Request& req, Response& resp)
                  {
                  Json::FastWriter writer;
                  Json::Value resp_json;
                   // 1. 获取blog_id 
                  int32_t blog_id = std::stoi(req.matches[1].str());
                  printf("删除id为 %d 的博客！\n", blog_id);
                  // 2. 调用数据库操作
                 bool ret = blog_table.Delete(blog_id);
                 if(!ret)
                 {
                     resp_json["ok"] = false;
                     resp_json["reason"] = "delete blog faile";
                     resp.status = 500;
                     resp.set_content(writer.write(resp_json), "application/json");
                   return;
                 }

                 // 3. 构造一个正确的响应
                 resp_json["ok"] = true;
                 resp.set_content(writer.write(resp_json), "application/json");
                 return;
                 });
                
    
    // 新增标签
    server.Post("/tag", [&tag_table](const Request& req, Response& resp)
                {
                Json::FastWriter writer;
                Json::Reader reader;
                Json::Value req_json;
                Json::Value resp_json;
                // 1. 解析请求
                bool ret = reader.parse(req.body, req_json);
                if(!ret)
                {
                    printf("插入标签失败,解析出错！\n");
                    resp_json["ok"] = false;
                    resp_json["reason"] = "insert tag request parse failed";
                    resp.status = 400;
                    resp.set_content(writer.write(resp_json), "application/json");
                    return;
                }

                // 2.对请求校验
                if(req_json["tag_name"].empty())
                {
                    printf("插入标签失败, 校验出错!\n");
                    resp_json["ok"] = false;
                    resp_json["reason"] = "insert tag request format faile";
                    resp.status = 400;
                    resp.set_content(writer.write(resp_json), "application/json");
                    return ;
                }

                // 3. 调用数据库操作完成插入
                ret = tag_table.Insert(req_json);
                if(!ret)
                {
                    printf("插入标签失败，数据库出错！\n");
                    resp_json["ok"] = false;
                    resp_json["reason"] = "insert tag database failed";
                    resp.status = 500;
                    resp.set_content(writer.write(resp_json), "application/json");
                    return ;
                }
                // 4. 返回正确结果
                resp_json["ok"] = true;
                resp.set_content(writer.write(resp_json), "application/json");
                return;
                });

    // 删除标签
    server.Delete(R"(/tag/(\d+))", [&tag_table](const Request& req, Response& resp)
                  {
                  Json::FastWriter writer;
                  Json::Value resp_json;
                  // 1. 解析tag_id
                  int32_t tag_id = std::stoi(req.matches[1].str());
                  printf("删除id为 %d 的标签！\n", tag_id);
                  // 2. 执行数据库操作
                  bool ret = tag_table.Delete(tag_id);
                  if(!ret)
                  {
                      printf("删除标签失败！\n");
                      resp_json["ok"] = false;
                      resp_json["reason"] = "delete tag database failed";
                      resp.status = 500;
                      resp.set_content(writer.write(resp_json), "application/json");
                      return ;
                }
                // 3. 构造正常的响应
                resp_json["ok"] = true;
                resp.set_content(writer.write(resp_json), "application/json");
                return;
                  });
    
    // 查看所有标签
    server.Get("/tag", [&tag_table](const Request& req, Response& resp)
               {
               // 1. 调用数据库操作
               Json::FastWriter writer;
               Json::Value resp_json;
               bool ret = tag_table.SelectAll(&resp_json);
               if(!ret)
               {
                    printf("获取标签失败！\n");
                    resp_json["ok"] = false;
                    resp_json["reason"] = "get tag database failed";
                    resp.status = 500;
                    resp.set_content(writer.write(resp_json), "application/json");
                    return ;
                }
                // 2. 构造正确响应
                resp.set_content(writer.write(resp_json), "application/json");
                return;
               });

    server.set_base_dir("./wwwroot");
    server.listen("0.0.0.0",9093);

    return 0;
}
