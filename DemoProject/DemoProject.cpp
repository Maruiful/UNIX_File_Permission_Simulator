// DemoProject.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../KernProject/user_manager.h"
#include "../KernProject/file_system.h"
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

// ==================== 第二阶段: 文件系统测试 ====================

// 文件系统状态打印函数
void print_fs_status(FileSystemStatus status) {
    switch (status) {
        case FS_SUCCESS:
            std::cout << "成功" << std::endl;
            break;
        case FS_ERR_EXISTS:
            std::cout << "失败: 文件已存在" << std::endl;
            break;
        case FS_ERR_NOT_FOUND:
            std::cout << "失败: 文件不存在" << std::endl;
            break;
        case FS_ERR_INVALID:
            std::cout << "失败: 参数无效" << std::endl;
            break;
        case FS_ERR_PERMISSION:
            std::cout << "失败: 权限不足" << std::endl;
            break;
        case FS_ERR_NOT_DIR:
            std::cout << "失败: 不是目录" << std::endl;
            break;
        case FS_ERR_NOT_EMPTY:
            std::cout << "失败: 目录非空" << std::endl;
            break;
        case FS_ERR_LIMIT:
            std::cout << "失败: 达到最大限制" << std::endl;
            break;
        default:
            std::cout << "失败: 未知错误" << std::endl;
            break;
    }
}

// 打印权限字符串 (rwxrwxrwx格式)
void print_permissions(Permissions perms) {
    // 所有者权限
    std::cout << ((perms.owner_perms & PERM_READ) ? "r" : "-");
    std::cout << ((perms.owner_perms & PERM_WRITE) ? "w" : "-");
    std::cout << ((perms.owner_perms & PERM_EXECUTE) ? "x" : "-");

    // 组权限
    std::cout << ((perms.group_perms & PERM_READ) ? "r" : "-");
    std::cout << ((perms.group_perms & PERM_WRITE) ? "w" : "-");
    std::cout << ((perms.group_perms & PERM_EXECUTE) ? "x" : "-");

    // 其他用户权限
    std::cout << ((perms.other_perms & PERM_READ) ? "r" : "-");
    std::cout << ((perms.other_perms & PERM_WRITE) ? "w" : "-");
    std::cout << ((perms.other_perms & PERM_EXECUTE) ? "x" : "-");
}

// 测试5: 文件系统初始化
void test_fs_init() {
    std::cout << "\n=== 测试5: 文件系统初始化 ===" << std::endl;

    // 初始化文件系统
    fs_init();

    // 验证根目录
    FileNode* root = fs_get_root();
    if (root) {
        std::cout << "根目录创建成功" << std::endl;
        std::cout << "  inode: " << root->inode << std::endl;
        std::cout << "  类型: " << (root->type == FILE_TYPE_DIRECTORY ? "目录" : "文件") << std::endl;
        std::cout << "  所有者: UID=" << root->uid << ", GID=" << root->gid << std::endl;
        std::cout << "  权限: ";
        print_permissions(root->perms);
        std::cout << std::endl;
    } else {
        std::cout << "错误: 根目录创建失败" << std::endl;
    }

    std::cout << "当前文件节点总数: " << fs_get_file_count() << std::endl;
}

// 测试6: 创建文件和目录
void test_fs_create() {
    std::cout << "\n=== 测试6: 创建文件和目录 ===" << std::endl;

    FileSystemStatus status;
    Permissions perms;

    // 在根目录创建文件 test.txt (所有者 alice)
    std::cout << "创建 /test.txt (所有者 alice, 权限 rw-r--r--): ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = PERM_READ;
    status = fs_create("/", "test.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // 创建目录 /home
    std::cout << "创建目录 /home (所有者 root, 权限 rwxr-xr-x): ";
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_EXECUTE;
    perms.other_perms = PERM_READ | PERM_EXECUTE;
    status = fs_create("/", "home", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    // 在 /home 下创建文件 readme.txt
    std::cout << "创建 /home/readme.txt (所有者 alice): ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = 0;
    status = fs_create("/home", "readme.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // 创建子目录 /home/user
    std::cout << "创建目录 /home/user (所有者 alice): ";
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_EXECUTE;
    perms.other_perms = PERM_READ | PERM_EXECUTE;
    status = fs_create("/home", "user", 1001, 100, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    // 在 /home/user 下创建文件 document.txt
    std::cout << "创建 /home/user/document.txt (所有者 bob): ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ | PERM_WRITE;
    perms.other_perms = 0;
    status = fs_create("/home/user", "document.txt", 1002, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // 尝试创建重复文件 (应该失败)
    std::cout << "再次创建 /test.txt (应该失败): ";
    status = fs_create("/", "test.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "\n当前文件节点总数: " << fs_get_file_count() << std::endl;
}

// 测试7: 路径查找
void test_fs_find() {
    std::cout << "\n=== 测试7: 路径查找 ===" << std::endl;

    FileNode* file;

    // 查找根目录
    std::cout << "查找根目录 \"/\" : ";
    file = fs_find("/");
    std::cout << (file ? "找到" : "未找到") << std::endl;

    // 查找已存在的文件
    std::cout << "查找 \"/test.txt\" : ";
    file = fs_find("/test.txt");
    if (file) {
        std::cout << "找到 - inode=" << file->inode << ", 类型=" << (file->type == FILE_TYPE_DIRECTORY ? "目录" : "文件") << std::endl;
    } else {
        std::cout << "未找到" << std::endl;
    }

    // 查找多级路径
    std::cout << "查找 \"/home/readme.txt\" : ";
    file = fs_find("/home/readme.txt");
    if (file) {
        std::cout << "找到 - inode=" << file->inode << ", 所有者UID=" << file->uid << std::endl;
    } else {
        std::cout << "未找到" << std::endl;
    }

    // 查找深层路径
    std::cout << "查找 \"/home/user/document.txt\" : ";
    file = fs_find("/home/user/document.txt");
    std::cout << (file ? "找到" : "未找到") << std::endl;

    // 查找目录
    std::cout << "查找目录 \"/home\" : ";
    file = fs_find("/home");
    if (file) {
        std::cout << "找到 - 类型=" << (file->type == FILE_TYPE_DIRECTORY ? "目录" : "文件") << std::endl;
    } else {
        std::cout << "未找到" << std::endl;
    }

    // 查找不存在的文件
    std::cout << "查找不存在的 \"/nonexistent.txt\" : ";
    file = fs_find("/nonexistent.txt");
    std::cout << (file ? "找到 (错误)" : "未找到 (正确)") << std::endl;

    // 查找不存在的多级路径
    std::cout << "查找不存在的 \"/home/abc/file.txt\" : ";
    file = fs_find("/home/abc/file.txt");
    std::cout << (file ? "找到 (错误)" : "未找到 (正确)") << std::endl;
}

// 测试8: 权限检查
void test_permission_check() {
    std::cout << "\n=== 测试8: 权限检查 ===" << std::endl;

    FileNode* file;
    PermissionResult result;

    // 查找测试文件 /test.txt (所有者 alice UID=1001)
    file = fs_find("/test.txt");
    if (!file) {
        std::cout << "错误: 找不到测试文件" << std::endl;
        return;
    }

    std::cout << "\n测试文件 /test.txt (所有者 alice UID=1001, GID=100):" << std::endl;
    std::cout << "权限: ";
    print_permissions(file->perms);
    std::cout << std::endl;

    // 测试所有者的读权限
    std::cout << "  alice (UID=1001) 读取文件: ";
    result = fs_check_permission(file, 1001, 100, OP_READ);
    std::cout << (result == PERM_GRANTED ? "允许" : "拒绝") << std::endl;

    // 测试所有者的写权限
    std::cout << "  alice (UID=1001) 写入文件: ";
    result = fs_check_permission(file, 1001, 100, OP_WRITE);
    std::cout << (result == PERM_GRANTED ? "允许" : "拒绝") << std::endl;

    // 测试所有者的执行权限
    std::cout << "  alice (UID=1001) 执行文件: ";
    result = fs_check_permission(file, 1001, 100, OP_EXECUTE);
    std::cout << (result == PERM_GRANTED ? "允许" : "拒绝") << std::endl;

    // 测试同组用户 bob (UID=1002, GID=100)
    std::cout << "\n  bob (UID=1002, 同组) 读取文件: ";
    result = fs_check_permission(file, 1002, 100, OP_READ);
    std::cout << (result == PERM_GRANTED ? "允许" : "拒绝") << std::endl;

    std::cout << "  bob (UID=1002, 同组) 写入文件: ";
    result = fs_check_permission(file, 1002, 100, OP_WRITE);
    std::cout << (result == PERM_GRANTED ? "允许" : "拒绝") << std::endl;

    // 测试其他用户 (假设有一个 UID=2000 的用户)
    std::cout << "\n  其他用户 (UID=2000) 读取文件: ";
    result = fs_check_permission(file, 2000, 2000, OP_READ);
    std::cout << (result == PERM_GRANTED ? "允许" : "拒绝") << std::endl;

    std::cout << "  其他用户 (UID=2000) 写入文件: ";
    result = fs_check_permission(file, 2000, 2000, OP_WRITE);
    std::cout << (result == PERM_GRANTED ? "允许" : "拒绝") << std::endl;

    // 测试 root 用户
    std::cout << "\n  root (UID=0) 删除文件: ";
    result = fs_check_permission(file, 0, 0, OP_DELETE);
    std::cout << (result == PERM_GRANTED ? "允许" : "拒绝") << std::endl;
}

// 测试9: 修改权限
void test_chmod() {
    std::cout << "\n=== 测试9: 修改文件权限 ===" << std::endl;

    FileSystemStatus status;
    FileNode* file;
    Permissions perms;

    // 查找文件
    file = fs_find("/test.txt");
    if (!file) {
        std::cout << "错误: 找不到测试文件" << std::endl;
        return;
    }

    std::cout << "原始权限: ";
    print_permissions(file->perms);
    std::cout << std::endl;

    // 修改为只读
    std::cout << "\n修改 /test.txt 为只读 (r--r--r--): ";
    perms.owner_perms = PERM_READ;
    perms.group_perms = PERM_READ;
    perms.other_perms = PERM_READ;
    status = fs_chmod("/test.txt", perms);
    print_fs_status(status);

    // 验证修改
    file = fs_find("/test.txt");
    if (file) {
        std::cout << "新权限: ";
        print_permissions(file->perms);
        std::cout << std::endl;

        // 测试写权限
        std::cout << "alice 尝试写入: ";
        PermissionResult result = fs_check_permission(file, 1001, 100, OP_WRITE);
        std::cout << (result == PERM_GRANTED ? "允许 (错误)" : "拒绝 (正确)") << std::endl;
    }

    // 恢复权限
    std::cout << "\n恢复权限为 rw-r--r--: ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = PERM_READ;
    status = fs_chmod("/test.txt", perms);
    print_fs_status(status);
}

// 测试10: 删除操作
void test_fs_delete() {
    std::cout << "\n=== 测试10: 删除操作 ===" << std::endl;

    FileSystemStatus status;
    FileNode* file;

    // 删除普通文件
    std::cout << "删除文件 /test.txt: ";
    status = fs_delete("/test.txt");
    print_fs_status(status);

    // 验证删除
    std::cout << "验证删除 - 查找 /test.txt: ";
    file = fs_find("/test.txt");
    std::cout << (file ? "仍存在 (错误)" : "已删除 (正确)") << std::endl;

    std::cout << "当前文件节点总数: " << fs_get_file_count() << std::endl;

    // 尝试删除非空目录 (应该失败)
    std::cout << "\n尝试删除非空目录 /home (应该失败): ";
    status = fs_delete("/home");
    print_fs_status(status);

    // 删除空目录
    std::cout << "\n创建一个空目录 /tmp: ";
    Permissions perms;
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.other_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    status = fs_create("/", "tmp", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "删除空目录 /tmp: ";
    status = fs_delete("/tmp");
    print_fs_status(status);

    // 验证删除
    std::cout << "验证删除 - 查找 /tmp: ";
    file = fs_find("/tmp");
    std::cout << (file ? "仍存在 (错误)" : "已删除 (正确)") << std::endl;

    std::cout << "\n当前文件节点总数: " << fs_get_file_count() << std::endl;
}

int main()
{
    SetConsoleOutputCP(65001);
    std::cout << "========================================" << std::endl;
    std::cout << "UNIX 文件权限模拟器 - 完整测试" << std::endl;
    std::cout << "========================================" << std::endl;

    // 初始化用户管理系统
    um_init();

    // 第一阶段: 用户和组管理测试
    std::cout << "\n############ 第一阶段: 用户和组管理测试 ############" << std::endl;
    test_basic_user_management();
    test_basic_group_management();
    test_group_membership();
    test_deletion();

    // 第二阶段: 文件系统测试
    std::cout << "\n############ 第二阶段: 文件系统测试 ############" << std::endl;
    test_fs_init();
    test_fs_create();
    test_fs_find();
    test_permission_check();
    test_chmod();
    test_fs_delete();

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
