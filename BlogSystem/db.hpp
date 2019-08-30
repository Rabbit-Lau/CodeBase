/*
    创建一些相关的类来封装数据库操作
*/
#pragma once
#include <cstdio>
#include <cstdlib>
#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>
#include <cstring>
#include <string>
#include <memory>
namespace blog_system
{
// 初始化，与数据库建立连接
static MYSQL* MySQLInit()
{
    // 1.  创建一个句柄
    MYSQL* connect_fd = mysql_init(NULL);
    // 2. 和数据库建立连接
    if(mysql_real_connect(connect_fd, "127.0.0.1", "root", "1225521", 
        "blog_system", 3306, NULL, 0) == NULL)
    {
        printf("连接失败！%s\n", mysql_error(connect_fd));
        return NULL;
    }
    // 3. 设定字符编码
    mysql_set_character_set(connect_fd, "utf8");
    return connect_fd;
}

// 断开连接
static void MySQLRelease(MYSQL* connect_fd)
{
    // 释放句柄并断开连接
    mysql_close(connect_fd);
}

// 创建BlogTable类，用于操作博客表
class BlogTable
{
public:
    BlogTable(MYSQL* connect_fd):_mysql(connect_fd)
    {
        // 通过构造函数获取数据库的句柄
    }

    // 数据库操作相关参数统一使用JSON的方式
    // Json::Value 表示一个具体的json对象
    // { 
    //      title: "博客标题"
    //      content: "博客正文"
    //      create_time: "创建时间"
    //      tag_id: "标签ID"
    // }
    bool Insert(const Json::Value& blog)
    {
        const std::string& content = blog["content"].asString();

        // 不使用 char* to = new char[content.size() * 2 + 1];
        // 使用智能指针，无需手动delete
        std::unique_ptr<char> to(new char[content.size() * 2 + 1]);
        mysql_real_escape_string(_mysql, to.get(), content.c_str(), content.size());
        // sql 保存sql语句
        std::unique_ptr<char> sql(new char[content.size() * 2 + 4096]);
        sprintf(sql.get(), "insert into blog_table values(null, '%s', '%s', %d, '%s')",
                            blog["title"].asCString(),
                            to.get(),
                            blog["tag_id"].asInt(),
                            blog["create_time"].asCString());
        int ret = mysql_query(_mysql, sql.get());
        if(ret != 0)
        {
            printf("插入博客失败！%s\n", mysql_error(_mysql));
            return false;
        }
        printf("插入博客成功！\n");
        return true;
    }
    
    // blogs作为一个输出型参数
    bool SelectAll(Json::Value* blogs, const std::string& tag_id = "")
    {
        char sql[1024 * 4] = {0};
        if(tag_id == "")
        {
            // 不需要按照tag来筛选结果
            sprintf(sql, "select blog_id, title, tag_id, create_time from blog_table");
        }
        else
        {
            // 需要暗转tag标签来筛选
            sprintf(sql, "select blog_id, title, tag_id, create_time from blog_table where tag_id = %d",
                          std::stoi(tag_id));
        }

        int ret = mysql_query(_mysql, sql);
        if(ret != 0)
        {
            printf("查找所有博客失败！%s\n", mysql_error(_mysql));
            return false;
        }
        
        MYSQL_RES* result = mysql_store_result(_mysql);// mysql_store_result()获取结果
        int rows = mysql_num_rows(result); // 获取行数
        //遍历结果集合，把结果集合写到blogs参数中，返回给调用者
        for(int i = 0; i < rows; ++i)
        {
            MYSQL_ROW row = mysql_fetch_row(result); //获取一行的数据
            Json::Value blog;
            blog["blog_id"] = atoi(row[0]);
            blog["title"] = row[1];
            blog["tag_id"] = atoi(row[2]);
            blog["create_time"] = row[3]; 
            blogs->append(blog); // 多个结果集合
        }
        mysql_free_result(result);
        printf("查找所有博客成功！共查到 %d 条博客。\n", rows);
        return true;
    }
    // blog作为一个输出型参数， 根据当前blog_id在数据库中
    // 找到具体的博客内容通过blog参数返回给调用者
    bool SelectOne(int32_t blog_id, Json::Value* blog)
    {
        char sql[1024] = {0};
        sprintf(sql, "select blog_id, title, content, tag_id, create_time from blog_table where blog_id = %d"
                        , blog_id);
        int ret = mysql_query(_mysql, sql);
        if(ret != 0)
        {
            printf("查找博客失败！%s\n", mysql_error(_mysql));
            return false;
        }
        MYSQL_RES* result = mysql_store_result(_mysql);
        int rows = mysql_num_rows(result);
        if(rows != 1)
        {
            printf("查找到的结果不止一条，实际有 %d 条。", rows);
            return false;
        }
        MYSQL_ROW row = mysql_fetch_row(result);
        (*blog)["blog_id"] = atoi(row[0]);
        (*blog)["title"] = row[1];
        (*blog)["content"] = row[2];
        (*blog)["tag_id"] = atoi(row[3]);
        (*blog)["create_time"] = row[4];
        return true;     
    }

    bool Update(const Json::Value& blog)
    {
        const std::string& content = blog["content"].asString();
        // 不使用 char* to = new char[content.size() * 2 + 1];
        // 使用智能指针，无需手动delete
        std::unique_ptr<char> to(new char[content.size() * 2 + 1]);
        mysql_real_escape_string(_mysql, to.get(), content.c_str(), content.size());
        
        // sql 保存sql语句
        std::unique_ptr<char> sql(new char[content.size() * 2 + 4096]);
        sprintf(sql.get(), "update blog_table set title='%s', content='%s', tag_id=%d where blog_id=%d",
                blog["title"].asCString(),
                to.get(),
                blog["tag_id"].asInt(),
                blog["blog_id"].asInt());
        int ret = mysql_query(_mysql, sql.get());
        if(ret != 0)
        {
            printf("更新博客失败!%s\n", mysql_error(_mysql));
            return false;
        }
        printf("更新博客成功!\n");
        return true;
    }

    bool Delete(int32_t blog_id)
    {
        char sql[1024*4] = {0};
        sprintf(sql, "delete from blog_table where blog_id = %d", blog_id);
        int ret = mysql_query(_mysql, sql);
        if(ret != 0)
        {
            printf("删除博客失败！%s\n", mysql_error(_mysql));
            return false;
        }
        printf("删除博客成功！\n");

        return true;
    }

private:
    MYSQL* _mysql;
};


// TagTable类，标签表的操作方法
class TagTable
{
public:
    TagTable(MYSQL* mysql): _mysql(mysql){}

    bool Insert(const Json::Value& tag)
    {
        char sql[1024 * 4];
        sprintf(sql, "insert into tag_table values(null, '%s')", tag["tag_name"].asCString());
        int ret = mysql_query(_mysql, sql);
        if(ret != 0)
        {
            printf("插入标签失败！%s\n", mysql_error(_mysql));
            return false;
        }
        printf("插入标签成功！\n");
        return true;
    }

    bool Delete(int32_t tag_id)
    {
        char sql[1024 * 4] = {0};
        sprintf(sql, "delete from tag_table where tag_id = %d", tag_id);
        int ret = mysql_query(_mysql, sql);
        if(ret != 0)
        {
            printf("删除标签失败！ %s\n", mysql_error(_mysql));
            return false;
        }
        printf("删除标签成功！\n");
        return true;
    }

    bool SelectAll(Json::Value* tags)
    {
        char sql[1024 * 4] = {0};
        sprintf(sql, "select tag_id, tag_name from tag_table");
        int ret = mysql_query(_mysql, sql);
        if(ret != 0)
        {
            printf("查找所有标签失败！%s\n", mysql_error(_mysql));
            return false;
        }
        MYSQL_RES* result = mysql_store_result(_mysql);
        int rows = mysql_num_rows(result);
        for(int i = 0; i < rows; i++)
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            Json::Value tag;
            tag["tag_id"] = atoi(row[0]);
            tag["tag_name"] = row[1];
            tags->append(tag);
        }
        printf("查找所有标签成功！共查找到 %d 个！\n", rows);
        return true;
    }

private:
    MYSQL* _mysql;
};


} // end blog_system
