#pragma once

#include "types.h"

// 用户管理函数

/**
 * 初始化用户管理系统
 * 在使用其他用户管理函数前必须调用
 */
void um_init(void);

/**
 * 创建新用户
 * @param username 用户名
 * @param uid 用户ID
 * @param gid 用户所属主组ID
 * @return 操作状态码
 */
UserStatus um_create_user(const char* username, int uid, int gid);

/**
 * 根据用户ID查找用户
 * @param uid 用户ID
 * @return 用户指针,未找到返回NULL
 */
User* um_find_user_by_uid(int uid);

/**
 * 根据用户名查找用户
 * @param username 用户名
 * @return 用户指针,未找到返回NULL
 */
User* um_find_user_by_name(const char* username);

/**
 * 获取用户数量
 * @return 当前系统中的用户总数
 */
int um_get_user_count(void);

/**
 * 删除用户
 * @param uid 要删除的用户ID
 * @return 操作状态码
 */
UserStatus um_delete_user(int uid);

// 用户组管理函数

/**
 * 创建新用户组
 * @param groupname 组名
 * @param gid 组ID
 * @return 操作状态码
 */
GroupStatus um_create_group(const char* groupname, int gid);

/**
 * 根据组ID查找用户组
 * @param gid 组ID
 * @return 组指针,未找到返回NULL
 */
Group* um_find_group_by_gid(int gid);

/**
 * 根据组名查找用户组
 * @param groupname 组名
 * @return 组指针,未找到返回NULL
 */
Group* um_find_group_by_name(const char* groupname);

/**
 * 向组中添加成员
 * @param gid 组ID
 * @param uid 要添加的用户ID
 * @return 操作状态码
 */
GroupStatus um_add_group_member(int gid, int uid);

/**
 * 从组中移除成员
 * @param gid 组ID
 * @param uid 要移除的用户ID
 * @return 操作状态码
 */
GroupStatus um_remove_group_member(int gid, int uid);

/**
 * 检查用户是否是组成员
 * @param gid 组ID
 * @param uid 用户ID
 * @return 1表示是成员,0表示不是成员
 */
int um_is_group_member(int gid, int uid);

/**
 * 获取组数量
 * @return 当前系统中的组总数
 */
int um_get_group_count(void);

/**
 * 删除用户组
 * @param gid 要删除的组ID
 * @return 操作状态码
 */
GroupStatus um_delete_group(int gid);
