// DemoProject.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../KernProject/user_manager.h"
#include <windows.h>

// 测试辅助函数:打印状态结果
void print_user_status(UserStatus status) {
    switch (status) {
        case USER_SUCCESS:
            std::cout << "成功" << std::endl;
            break;
        case USER_ERR_EXISTS:
            std::cout << "失败: 用户已存在" << std::endl;
            break;
        case USER_ERR_NOT_FOUND:
            std::cout << "失败: 用户不存在" << std::endl;
            break;
        case USER_ERR_INVALID:
            std::cout << "失败: 参数无效" << std::endl;
            break;
        case USER_ERR_LIMIT:
            std::cout << "失败: 达到最大限制" << std::endl;
            break;
        default:
            std::cout << "失败: 未知错误" << std::endl;
            break;
    }
}

void print_group_status(GroupStatus status) {
    switch (status) {
        case GROUP_SUCCESS:
            std::cout << "成功" << std::endl;
            break;
        case GROUP_ERR_EXISTS:
            std::cout << "失败: 组已存在" << std::endl;
            break;
        case GROUP_ERR_NOT_FOUND:
            std::cout << "失败: 组不存在" << std::endl;
            break;
        case GROUP_ERR_INVALID:
            std::cout << "失败: 参数无效" << std::endl;
            break;
        case GROUP_ERR_LIMIT:
            std::cout << "失败: 达到最大限制" << std::endl;
            break;
        default:
            std::cout << "失败: 未知错误" << std::endl;
            break;
    }
}

// 测试1: 基本用户管理
void test_basic_user_management() {
    std::cout << "\n=== 测试1: 基本用户管理 ===" << std::endl;

    UserStatus status;

    // 创建用户
    std::cout << "创建用户 root (uid=0, gid=0): ";
    status = um_create_user("root", 0, 0);
    print_user_status(status);

    std::cout << "创建用户 alice (uid=1001, gid=100): ";
    status = um_create_user("alice", 1001, 100);
    print_user_status(status);

    std::cout << "创建用户 bob (uid=1002, gid=100): ";
    status = um_create_user("bob", 1002, 100);
    print_user_status(status);

    std::cout << "创建用户 charlie (uid=1003, gid=101): ";
    status = um_create_user("charlie", 1003, 101);
    print_user_status(status);

    // 尝试创建重复用户
    std::cout << "再次创建 alice (应该失败): ";
    status = um_create_user("alice", 1001, 100);
    print_user_status(status);

    // 查找用户
    std::cout << "\n查找用户测试:" << std::endl;
    User* user = um_find_user_by_uid(1001);
    if (user) {
        std::cout << "找到用户: " << user->username
                  << " (UID=" << user->uid
                  << ", GID=" << user->gid << ")" << std::endl;
    }

    user = um_find_user_by_name("root");
    if (user) {
        std::cout << "通过名字找到用户: " << user->username
                  << " (UID=" << user->uid << ")" << std::endl;
    }

    user = um_find_user_by_name("nonexistent");
    if (!user) {
        std::cout << "查找不存在的用户: 未找到 (正确)" << std::endl;
    }

    // 显示用户总数
    std::cout << "\n当前用户总数: " << um_get_user_count() << std::endl;
}

// 测试2: 基本用户组管理
void test_basic_group_management() {
    std::cout << "\n=== 测试2: 基本用户组管理 ===" << std::endl;

    GroupStatus status;

    // 创建用户组
    std::cout << "创建组 root (gid=0): ";
    status = um_create_group("root", 0);
    print_group_status(status);

    std::cout << "创建组 developers (gid=100): ";
    status = um_create_group("developers", 100);
    print_group_status(status);

    std::cout << "创建组 managers (gid=101): ";
    status = um_create_group("managers", 101);
    print_group_status(status);

    // 尝试创建重复组
    std::cout << "再次创建 developers (应该失败): ";
    status = um_create_group("developers", 100);
    print_group_status(status);

    // 查找组
    std::cout << "\n查找组测试:" << std::endl;
    Group* group = um_find_group_by_gid(100);
    if (group) {
        std::cout << "找到组: " << group->groupname
                  << " (GID=" << group->gid << ")" << std::endl;
    }

    group = um_find_group_by_name("managers");
    if (group) {
        std::cout << "通过名字找到组: " << group->groupname
                  << " (GID=" << group->gid << ")" << std::endl;
    }

    // 显示组总数
    std::cout << "\n当前组总数: " << um_get_group_count() << std::endl;
}

// 测试3: 组成员管理
void test_group_membership() {
    std::cout << "\n=== 测试3: 组成员管理 ===" << std::endl;

    GroupStatus status;

    // 添加成员到组
    std::cout << "添加 alice 到 developers 组: ";
    status = um_add_group_member(100, 1001);
    print_group_status(status);

    std::cout << "添加 bob 到 developers 组: ";
    status = um_add_group_member(100, 1002);
    print_group_status(status);

    std::cout << "添加 charlie 到 managers 组: ";
    status = um_add_group_member(101, 1003);
    print_group_status(status);

    // 尝试添加重复成员
    std::cout << "再次添加 alice 到 developers (应该失败或已存在): ";
    status = um_add_group_member(100, 1001);
    print_group_status(status);

    // 检查成员关系
    std::cout << "\n检查成员关系:" << std::endl;
    std::cout << "alice 是否在 developers 组? "
              << (um_is_group_member(100, 1001) ? "是" : "否") << std::endl;
    std::cout << "bob 是否在 developers 组? "
              << (um_is_group_member(100, 1002) ? "是" : "否") << std::endl;
    std::cout << "charlie 是否在 developers 组? "
              << (um_is_group_member(100, 1003) ? "是" : "否") << std::endl;
    std::cout << "root 是否在 developers 组? "
              << (um_is_group_member(100, 0) ? "是" : "否") << std::endl;

    // 移除成员
    std::cout << "\n移除 bob 从 developers 组: ";
    status = um_remove_group_member(100, 1002);
    print_group_status(status);

    std::cout << "再次检查 bob 是否在 developers 组: "
              << (um_is_group_member(100, 1002) ? "是" : "否") << std::endl;

    // 显示组成员数
    Group* group = um_find_group_by_gid(100);
    if (group) {
        std::cout << "developers 组的成员数: " << group->member_count << std::endl;
    }
}

// 测试4: 删除操作
void test_deletion() {
    std::cout << "\n=== 测试4: 删除操作 ===" << std::endl;

    std::cout << "删除用户 charlie: ";
    UserStatus status = um_delete_user(1003);
    print_user_status(status);

    std::cout << "用户总数: " << um_get_user_count() << std::endl;

    std::cout << "验证 charlie 是否已删除: ";
    User* user = um_find_user_by_uid(1003);
    std::cout << (user ? "仍存在 (错误)" : "已删除 (正确)") << std::endl;

    std::cout << "\n删除 managers 组: ";
    GroupStatus gstatus = um_delete_group(101);
    print_group_status(gstatus);

    std::cout << "组总数: " << um_get_group_count() << std::endl;

    std::cout << "验证 managers 组是否已删除: ";
    Group* group = um_find_group_by_gid(101);
    std::cout << (group ? "仍存在 (错误)" : "已删除 (正确)") << std::endl;
}

int main()
{
    SetConsoleOutputCP(65001);
    std::cout << "========================================" << std::endl;
    std::cout << "UNIX 文件权限模拟器 - 用户管理测试" << std::endl;
    std::cout << "========================================" << std::endl;

    // 初始化用户管理系统
    um_init();

    // 运行测试
    test_basic_user_management();
    test_basic_group_management();
    test_group_membership();
    test_deletion();

    std::cout << "\n========================================" << std::endl;
    std::cout << "所有测试完成!" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
