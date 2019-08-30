#include "db.hpp"

void TestBlogTable()
{
    Json::StyledWriter writer;
    MYSQL* mysql = blog_system::MySQLInit();
    blog_system::BlogTable blog_table(mysql);
    bool ret = false;
    Json::Value blog;
    /*
    Json::Value blog;
    blog["title"] = "将进酒";
    blog["content"]= "将进酒， 杯莫停！";
    blog["tag_id"] = 1;
    blog["create_time"] = "2019/07/28";
    
    ret = blog_table.Insert(blog);
    printf("insert: %d", ret);
    */ 
    // 测试查找
    
    //Json::Value blogs;
    //ret = blog_table.SelectAll(&blogs);
   // printf("select all %d\n", ret);
    //printf("blogs:%s\n",writer.write(blogs).c_str());
 

    // 测试查找一个博
    ret = blog_table.SelectOne(6, &blog);
    printf("select one %d\n", ret);
    printf("blog:%s\n",writer.write(blog).c_str());

    /*
    // 测试修改博客
    blog["blog_id"] = 1;
    blog["title"]= "虞美人";
    blog["content"] = "春花秋月何时了，\n 往事知多少？";
    ret = blog_table.Update(blog);
    printf("Update: %d\n", ret);
    */
    /*
    // 测试删除
    ret = blog_table.Delete(1);
    printf("Delete:%d", ret);
    */
    blog_system::MySQLRelease(mysql);
}

void testTagTable()
{
    Json::StyledWriter writer;
    MYSQL* mysql = blog_system::MySQLInit();
    blog_system::TagTable tag_table(mysql);
    bool ret = false;
    
    // 测试插入
    //Json::Value tag;
    //tag["tag_name"] = "C语言";
    //ret = tag_table.Insert(tag);
    //printf("insert: %d\n", ret);
    //blog_system::MySQLRelease(mysql);
    
    //测试删除
    //ret = tag_table.Delete(3);
    //printf("Delete: %d", ret);
    
    // 测试查找
    Json::Value tags;
    ret = tag_table.SelectAll(&tags);
    printf("SelectAll: %d\n", ret);
    printf("tags:%s\n", writer.write(tags).c_str());

    blog_system::MySQLRelease(mysql);
}


int main()
{
    TestBlogTable();
    //testTagTable();
        return 0;
}

