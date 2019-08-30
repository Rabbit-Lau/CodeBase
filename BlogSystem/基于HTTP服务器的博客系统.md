## 基于HTTP服务器的博客系统

### 一、需求分析

实现一个简单的博客系统。

* 只支持单个用户；
* 实现对博客文章的增删改查；
* 实现对博客标签的增删改查。

### 二、模块划分

系统主要分为三个模块，分别为客户端、服务器、数据库。客户端主要是通过网页的形式来呈现，服务器端使用HTTP协议与客户端通信，数据库使用MySQL来管理数据。

![mokuai](C:\Users\LXF\Desktop\mokuai.png)

**博客服务器：**

（1）完成对博客的增删改查。

（2）完成对标签的增删改查吧。

**博客客户端：**

（1）博客标题列表页面。

（2）博客内容展示页面。

（3）博客管理页面。

（4）博客内容编辑页面。

例如：当用户在客户端(网页)执行某个操作的时候，比如新增博客，此时就会由客户端给服务器发送HTTP请求，该请求包含了用户的行为，服务器再根据这个行为来完成对数据的操作(数据库操作)。

### 三、设计思路

设计步骤如下：

1. 设计数据库。即设计数据库表结构，后续操作以数据库为基础；
2. 设计前后端交互接口。即客户端与服务器端如何交互，HTTP协议的方法选择，body部分数据的设计；
3. 实现服务器端和客户端的逻辑。

### 四、数据库设计

#### 4.1 创建数据库与表

首先，创建一个数据库。然后在该数据库中创建两个表，分别为博客表、标签表。

~~~ Mysql
// 创建博客系统数据库，并使用该数据库。
create database if not exists blog_system;
use blog_system;

// 创建博客表
drop table if exists blog_table;
create table blog_table(
    blog_id int not null primary key auto_increment,  // id 自增主键
	title varchar(50),
	content text,
	tag_id int,
	create_time varchar(50));

// 创建标签表
drop table if exists tag_table
create table tag_table(tag_id int not null primary key auto_increment, //id 自增主键
tag_name varchar(50));
~~~

**表结构如下：**

**博客表**

![blog_table](C:\Users\LXF\Desktop\blog_table.png)

**标签表**

![tag_table](C:\Users\LXF\Desktop\tag_table.png)



#### 4.2 MySQL API

我们需要通过服务器来完成对数据库的操作，使用 MySQL C API 操作数据库即可，以下为需要用到的MySQL API。

~~~C++
// 初始化
1. MYSQL *mysql_init(MYSQL *mysql)
// 参数：NULL
~~~

分配或初始化适用于`mysql_real_connect()`的 MYSQL 对象。如果 mysql 是 NULL 指针，则该函数分配、初始化并返回一个新对象。否则，初始化对象并返回对象的地址。如果`mysql_init()`分配一个新对象，则在调用`mysql_close()`关闭连接时将释放该对象。

~~~C++
// 建立连接
2.MYSQL *mysql_real_connect(MYSQL *mysql, const char *host, const char *user, const 							char *passwd, const char *db, unsigned int port, const char 							*unix_socket, unsigned long client_flag)
    
// 参数：a) 连接句柄。mysql_init 的返回值。
//    	b) 服务器的ip地址。
//    	c) 用户名
//    	d) 密码
//    	e) 数据库名
//    	f) 服务器的端口号
//    	g) unix_sock NULL
//    	h) client_flag 0
~~~

`mysql_real_connect()`尝试建立与主机上运行的 MySQL 数据库引擎的连接。 `mysql_real_connect()`必须成功完成才能执行任何其他需要有效 MYSQL 连接处理程序结构的 API 函数。

~~~ C
// 设置编码格式
3. int mysql_set_character_set(MYSQL *mysql, const char *csname) 
// 参数：a) mysql_init 的返回值
//  	b) 编码格式
~~~

`mysql_set_character_set()`此函数用于设置当前连接的默认字符集。

~~~C++
//  5. 执行 SQL 语句
4. int mysql_query(MYSQL *mysql, const char *stmt_str)
// 参数：a) mysql_init 返回值
//   	b) 存放sql语句的字符串，该字符串不含(;)
~~~

`mysql_query`执行以 nul l结尾的字符串 stmt_str 指向的 SQL 语句。通常，字符串必须由单个 SQL 语句组成，不带终止分号（;）。

~~~C++
// 关闭句柄
5. void mysql_close(MYSQL *mysql)
~~~

关闭先前打开的连接。如果处理程序是`mysql_init()`或`mysql_connect()`自动分配的，`mysql_close()`还会释放mysql指向的连接处理程序。关闭后不要使用处理程序。

~~~c++
// 由于博客正文content中可能会出现一些特殊字符，所以需要转义。
6. unsigned long mysql_real_escape_string(MYSQL *mysql, char *to, const char *from, 											unsigned long length)
// 参数：a) mysql_init 返回值
//   	b) 存放转义后的字符串 大小为2 * content.length() + 1
//    	c) 转义前的字符串
//      d) content.size()
~~~

此函数创建一个合法的 SQL 字符串，以便在 SQL 语句中使用mysql参数必须是有效的开放连接，因为字符转义取决于服务器使用的字符集。对 from 参数中的字符串进行编码以生成转义的 SQL字符串，同时考虑连接的当前字符集。结果放在 to 参数中，后跟一个终止空字节。mysql 参数必须是有效的开放连接，因为字符转义取决于服务器使用的字符集。 from 指向的字符串必须是 length 个字节长。您必须将 to buffer 分配为至少长度为* 2 + 1个字节的长度。（在最坏的情况下，每个字符可能需要使用两个字节进行编码，并且必须有终止空字节的空间。）当`mysql_real_escape_string()`返回时，to 的内容是以空字符结尾的字符串。返回值是编码字符串的长度，不包括终止空字节。

~~~c++
// 获取结果集合
7. MYSQL_RES *mysql_store_result(MYSQL *mysql)
// 参数：mysql_init()返回值
~~~

在调用`mysql_query()`或`mysql_real_query()`之后，必须为每个成功生成结果集的语句（SELECT，SHOW，DESCRIBE，EXPLAIN，CHECK TABLE等）调用`mysql_store_result()`或 `mysql_use_result()`。完成结果集后，还必须调用`mysql_free_result()`。

~~~C++
// 获取结果集中的行数
8. my_ulonglong mysql_num_rows(MYSQL_RES *result)
// 参数： mysql_store_result()的返回值
~~~

`mysql_num_rows()`的使用取决于是使用`mysql_store_result()`还是`mysql_use_result()`来返回结果集。如果使用`mysql_store_result()`可以立即调用`mysql_num_rows()`。如果使用`mysql_use_result()`，则在检索到结果集中的所有行之前，`mysql_num_rows()`不会返回正确的值。`mysql_num_rows()`旨在用于返回结果集的语句，例如SELECT。对于INSERT，UPDATE或DELETE等语句，可以使用`mysql_affected_rows()`获取受影响的行数。

~~~c++
// 获取到一行
9. MYSQL_ROW mysql_fetch_row(MYSQL_RES *result)
// 参数：mysql_store_result()的返回值。
~~~

`mysql_fetch_row()`检索结果集的下一行。

### 五、前后端交互接口

设计前后端的交互API，基于HTTP协议进行扩展，创建自定制协议，我们使用 RESTful （表征性状态转移）风格的 API 设计方式。参考[【RESTful API 设计指南】](<http://www.ruanyifeng.com/blog/2014/05/restful_api.html>)。

使用不同的HTTP方法来表达不同的语义，

- GET（SELECT）：从服务器取出资源（一项或多项）。
- POST（CREATE）：在服务器新建一个资源。
- PUT（UPDATE）：在服务器更新资源（客户端提供改变后的完整资源）。
- PATCH（UPDATE）：在服务器更新资源（客户端提供改变的属性，项目未涉及）。
- DELETE（DELETE）：从服务器删除资源。

使用 path 表示要操作的资源，如：

* GET /blog ：获取（列出）所有博客。
* GET /tag ：新建一个博客。

使用 json 来组织 body 中的数据，如：

~~~json
{
	title: "xxxx",
	content: "xxxx",
	create_time: "xxxx",
	tag_id: "xxxx",
}
~~~

响应时，服务器向用户返回状态码和提示信息。

- 200 OK - [GET]：服务器成功返回用户请求的数据，该操作是幂等的（Idempotent）。
- 201 CREATED - [POST/PUT/PATCH]：用户新建或修改数据成功。
- 202 Accepted - [*]：表示一个请求已经进入后台排队（异步任务）
- 204 NO CONTENT - [DELETE]：用户删除数据成功。
- 400 INVALID REQUEST - [POST/PUT/PATCH]：用户发出的请求有错误，服务器没有进行新建或修改数据的操作，该操作是幂等的。
- 401 Unauthorized - [*]：表示用户没有权限（令牌、用户名、密码错误）。
- 403 Forbidden - [*] 表示用户得到授权（与401错误相对），但是访问是被禁止的。
- 404 NOT FOUND - [*]：用户发出的请求针对的是不存在的记录，服务器没有进行操作，该操作是幂等的。
- 406 Not Acceptable - [GET]：用户请求的格式不可得（比如用户请求JSON格式，但是只有XML格式）。
- 410 Gone -[GET]：用户请求的资源被永久删除，且不会再得到的。
- 422 Unprocesable entity - [POST/PUT/PATCH] 当创建一个对象时，发生一个验证错误。
- 500 INTERNAL SERVER ERROR - [*]：服务器发生错误，用户将无法判断发出的请求是否成功。

#### 5.1 博客管理

**新增博客**

用于写博客，发布博客。

请求：

~~~http 
POST /blog
{
	title: "xxxx",
	content: "xxxx",
	create_time: "xxxx",
	tag_id: "xxxx",
}
~~~

响应：

~~~http
HTTP/1.1 200 OK
{
    ok: true
    reason: "xxxx"
}
~~~

**查看所有博客（标题列表）**

用于展示博客。

请求：

~~~http
GET /blog  // 获取所有
GET /blog?tag_id=1  //按照标签来筛选
~~~

响应：

~~~http
HTTP/1.1 200 OK
[
    {
        blog_id: 1,
        title: "xxxx",
        create_time: "xxxx",
        tag_id: 1
    },
    {
        
    }
]
~~~

**查看某个博客**

用于查看博客详细内容。

请求：

~~~http
GET /blog/:blog_id   // blog_id 为数字
~~~

响应：

~~~http
HTTP/1.1 200 OK
{
 	blog_id: 1,
    title: "xxxx",
    content: "xxxx",
    create_time: "xxxx",
    tag_id: xxxx,
}
~~~

**删除博客**

用于删除博客。

请求：

~~~http
DELETE /blog/:blog_id
~~~

响应：

~~~http
HTTP/1.1 200 OK
{
    ok:true
}
~~~

**修改博客**

用于编辑博客。

请求：

~~~http
PUT /blog/:blog_id
{
    title: "xxxx",
    content: "xxxx",
    tag_id: "xxxx"
}
~~~

响应：

~~~http
HTTP/1.1 200 OK
{
    ok:true
}
~~~



#### 5.2 标签管理

**新增标签**

用于添加新标签。

请求：

~~~http
POST /tag
{
	"tag_name":  新增标签名
}
~~~

响应：

~~~http
http/1.1 200 OK
{
	ok:true 
}
~~~

**查看所有标签**

用于查看所有标签。

请求：

~~~http
GET /tag
~~~

响应：

~~~http
HTTP/1.1 200 OK
[
    {
        tag_id: "",
        tag_name: ""
    },
    {
        tag_id: "",
        tag_name: ""
    }
]
~~~

**删除标签**

用于删除某个标签。

请求：

~~~http
DELETE /tag/:tag_id
~~~

响应：

~~~http
HTTP/1.1 200 OK
{
    ok: true
}
~~~

#### 5.3 服务器端代码

数据库操作见：db.hpp

HTTP服务器见：blog_server.cpp

注：HTTP使用第三方库 [httplib](<https://github.com/yhirose/cpp-httplib>)。



### 六、客户端实现

由于未曾系统学习前端知识，自己制作前端网页着实困难，所以使用模板进行更改。

网页中有些内容需要动态显示，所以使用 **Vue.js** ，数据交互部分使用**JQuery ajax**来实现。

