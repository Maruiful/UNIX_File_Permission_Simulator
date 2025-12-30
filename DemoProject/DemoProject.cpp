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

    // root 在根目录创建目录 /alice (所有者 root, 权限 rwxrwxrwx)
    std::cout << "root 创建目录 /alice (所有者 root, 权限 rwxrwxrwx): ";
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.other_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    status = fs_create("/", "alice", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    // root 修改 /alice 的所有者为 alice
    std::cout << "root 将 /alice 的所有者改为 alice: ";
    status = fs_chown("/alice", 1001, 100);
    print_fs_status(status);

    // root 修改 /alice 的权限为 rwx------ (只有 alice 可访问)
    std::cout << "root 修改 /alice 的权限为 rwx------: ";
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = 0;
    perms.other_perms = 0;
    status = fs_chmod("/alice", perms);
    print_fs_status(status);

    // alice 在自己的目录下创建文件
    std::cout << "alice 创建 /alice/test.txt (权限 rw-r--r--): ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = PERM_READ;
    status = fs_create("/alice", "test.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // root 创建目录 /home (权限 rwxrwxrwx)
    std::cout << "root 创建目录 /home (权限 rwxrwxrwx): ";
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.other_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    status = fs_create("/", "home", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    // alice 在 /home 下创建文件 readme.txt
    std::cout << "alice 创建 /home/readme.txt: ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = 0;
    status = fs_create("/home", "readme.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // 创建子目录 /home/user (所有者 alice)
    std::cout << "alice 创建目录 /home/user: ";
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_EXECUTE;
    perms.other_perms = PERM_READ | PERM_EXECUTE;
    status = fs_create("/home", "user", 1001, 100, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    // alice 在 /home/user 下创建文件 document.txt (所有者 bob)
    std::cout << "alice 创建 /home/user/document.txt (所有者 bob): ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ | PERM_WRITE;
    perms.other_perms = 0;
    status = fs_create("/home/user", "document.txt", 1002, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // 尝试创建重复文件 (应该失败)
    std::cout << "再次创建 /alice/test.txt (应该失败): ";
    status = fs_create("/alice", "test.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
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
    std::cout << "查找 \"/alice/test.txt\" : ";
    file = fs_find("/alice/test.txt");
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

    // 查找测试文件 /alice/test.txt (所有者 alice UID=1001)
    file = fs_find("/alice/test.txt");
    if (!file) {
        std::cout << "错误: 找不到测试文件" << std::endl;
        return;
    }

    std::cout << "\n测试文件 /alice/test.txt (所有者 alice UID=1001, GID=100):" << std::endl;
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
    file = fs_find("/alice/test.txt");
    if (!file) {
        std::cout << "错误: 找不到测试文件" << std::endl;
        return;
    }

    std::cout << "原始权限: ";
    print_permissions(file->perms);
    std::cout << std::endl;

    // 修改为只读
    std::cout << "\n修改 /alice/test.txt 为只读 (r--r--r--): ";
    perms.owner_perms = PERM_READ;
    perms.group_perms = PERM_READ;
    perms.other_perms = PERM_READ;
    status = fs_chmod("/alice/test.txt", perms);
    print_fs_status(status);

    // 验证修改
    file = fs_find("/alice/test.txt");
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
    status = fs_chmod("/alice/test.txt", perms);
    print_fs_status(status);
}

// 测试10: 删除操作
void test_fs_delete() {
    std::cout << "\n=== 测试10: 删除操作 ===" << std::endl;

    FileSystemStatus status;
    FileNode* file;

    // 删除普通文件
    std::cout << "删除文件 /alice/test.txt: ";
    status = fs_delete("/alice/test.txt");
    print_fs_status(status);

    // 验证删除
    std::cout << "验证删除 - 查找 /alice/test.txt: ";
    file = fs_find("/alice/test.txt");
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

// ==================== 第三阶段: 文件移动和重命名测试 ====================

// 测试11: 文件重命名
void test_rename() {
    std::cout << "\n=== 测试11: 文件重命名 ===" << std::endl;

    FileSystemStatus status;
    FileNode* file;
    Permissions perms;

    // 首先创建一个测试文件
    std::cout << "创建测试文件 /home/oldname.txt: ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = 0;
    status = fs_create("/home", "oldname.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // 验证文件创建成功
    std::cout << "查找 /home/oldname.txt: ";
    file = fs_find("/home/oldname.txt");
    std::cout << (file ? "找到" : "未找到") << std::endl;

    // 重命名文件 (使用 root 权限)
    std::cout << "\nroot 重命名 /home/oldname.txt 为 newname.txt: ";
    status = fs_rename("/home/oldname.txt", "newname.txt", 0, 0);
    print_fs_status(status);

    // 验证重命名成功
    std::cout << "查找 /home/oldname.txt: ";
    file = fs_find("/home/oldname.txt");
    std::cout << (file ? "仍存在 (错误)" : "已不存在 (正确)") << std::endl;

    std::cout << "查找 /home/newname.txt: ";
    file = fs_find("/home/newname.txt");
    std::cout << (file ? "找到 (正确)" : "未找到 (错误)") << std::endl;

    // 尝试重命名为已存在的文件名 (应该失败)
    std::cout << "\n创建文件 /home/another.txt: ";
    status = fs_create("/home", "another.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "尝试将 newname.txt 重命名为 another.txt (应该失败): ";
    status = fs_rename("/home/newname.txt", "another.txt", 0, 0);
    print_fs_status(status);

    // 尝试非所有者重命名 (权限测试)
    std::cout << "\n尝试由 bob (UID=1002) 重命名 newname.txt (所有者是 alice): ";
    status = fs_rename("/home/newname.txt", "bobname.txt", 1002, 100);
    print_fs_status(status);

    // 尝试重命名根目录 (应该失败)
    std::cout << "\n尝试重命名根目录 / (应该失败): ";
    status = fs_rename("/", "root", 0, 0);
    print_fs_status(status);

    // 清理
    std::cout << "\n清理测试文件:" << std::endl;
    std::cout << "删除 /home/newname.txt: ";
    status = fs_delete("/home/newname.txt");
    print_fs_status(status);
    std::cout << "删除 /home/another.txt: ";
    status = fs_delete("/home/another.txt");
    print_fs_status(status);
}

// 测试12: 目录重命名
void test_rename_directory() {
    std::cout << "\n=== 测试12: 目录重命名 ===" << std::endl;

    FileSystemStatus status;
    FileNode* dir;
    Permissions perms;

    // 创建测试目录
    std::cout << "创建测试目录 /olddir: ";
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_EXECUTE;
    perms.other_perms = 0;
    status = fs_create("/", "olddir", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    // 在目录中创建一个文件
    std::cout << "在 /olddir 中创建文件 test.txt: ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = 0;
    status = fs_create("/olddir", "test.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // 重命名目录
    std::cout << "\n重命名目录 /olddir 为 newdir: ";
    status = fs_rename("/olddir", "newdir", 0, 0);
    print_fs_status(status);

    // 验证目录重命名成功
    std::cout << "查找 /olddir: ";
    dir = fs_find("/olddir");
    std::cout << (dir ? "仍存在 (错误)" : "已不存在 (正确)") << std::endl;

    std::cout << "查找 /newdir: ";
    dir = fs_find("/newdir");
    std::cout << (dir ? "找到 (正确)" : "未找到 (错误)") << std::endl;

    // 验证目录中的文件仍然可以访问
    std::cout << "查找 /newdir/test.txt: ";
    FileNode* file = fs_find("/newdir/test.txt");
    std::cout << (file ? "找到 (正确)" : "未找到 (错误)") << std::endl;

    // 清理
    std::cout << "\n清理测试目录:" << std::endl;
    std::cout << "删除 /newdir/test.txt: ";
    status = fs_delete("/newdir/test.txt");
    print_fs_status(status);
    std::cout << "删除 /newdir: ";
    status = fs_delete("/newdir");
    print_fs_status(status);
}

// 测试13: 移动文件
void test_move_file() {
    std::cout << "\n=== 测试13: 移动文件 ===" << std::endl;

    FileSystemStatus status;
    FileNode* file;
    Permissions perms;

    // 创建测试文件在 /home 目录
    std::cout << "在 /home 创建文件 move_test.txt: ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = 0;
    status = fs_create("/home", "move_test.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // 移动文件到 /alice 目录
    std::cout << "\nroot 移动 /home/move_test.txt 到 /alice 目录: ";
    status = fs_move("/home/move_test.txt", "/alice", 0, 0);
    print_fs_status(status);

    // 验证移动成功
    std::cout << "查找 /home/move_test.txt: ";
    file = fs_find("/home/move_test.txt");
    std::cout << (file ? "仍存在 (错误)" : "已不存在 (正确)") << std::endl;

    std::cout << "查找 /alice/move_test.txt: ";
    file = fs_find("/alice/move_test.txt");
    std::cout << (file ? "找到 (正确)" : "未找到 (错误)") << std::endl;

    // 尝试移动到不存在的目录 (应该失败)
    std::cout << "\n创建另一个测试文件: ";
    status = fs_create("/home", "test2.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "尝试移动到不存在的目录 /nonexistent (应该失败): ";
    status = fs_move("/home/test2.txt", "/nonexistent", 0, 0);
    print_fs_status(status);

    // 尝试移动到目标位置已存在同名文件 (应该失败)
    std::cout << "\n在 /alice 创建文件 duplicate.txt: ";
    status = fs_create("/alice", "duplicate.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "在 /home 创建文件 duplicate.txt: ";
    status = fs_create("/home", "duplicate.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "尝试移动 /home/duplicate.txt 到 /alice (同名文件已存在，应该失败): ";
    status = fs_move("/home/duplicate.txt", "/alice", 0, 0);
    print_fs_status(status);

    // 尝试移动到非目录 (应该失败)
    std::cout << "\n尝试移动 /home/duplicate.txt 到 /alice/duplicate.txt (不是目录，应该失败): ";
    status = fs_move("/home/duplicate.txt", "/alice/duplicate.txt", 0, 0);
    print_fs_status(status);

    // 清理
    std::cout << "\n清理测试文件:" << std::endl;
    std::cout << "删除 /alice/move_test.txt: ";
    status = fs_delete("/alice/move_test.txt");
    print_fs_status(status);
    std::cout << "删除 /alice/duplicate.txt: ";
    status = fs_delete("/alice/duplicate.txt");
    print_fs_status(status);
    std::cout << "删除 /home/test2.txt: ";
    status = fs_delete("/home/test2.txt");
    print_fs_status(status);
    std::cout << "删除 /home/duplicate.txt: ";
    status = fs_delete("/home/duplicate.txt");
    print_fs_status(status);
}

// 测试14: 移动目录
void test_move_directory() {
    std::cout << "\n=== 测试14: 移动目录 ===" << std::endl;

    FileSystemStatus status;
    FileNode* dir;
    Permissions perms;

    // 创建目录结构: /source 和 /source/file.txt
    std::cout << "创建目录 /source: ";
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_EXECUTE;
    perms.other_perms = 0;
    status = fs_create("/", "source", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "在 /source 创建文件 file.txt: ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = 0;
    status = fs_create("/source", "file.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // 移动整个目录到 /home
    std::cout << "\n移动目录 /source 到 /home: ";
    status = fs_move("/source", "/home", 0, 0);
    print_fs_status(status);

    // 验证移动成功
    std::cout << "查找 /source: ";
    dir = fs_find("/source");
    std::cout << (dir ? "仍存在 (错误)" : "已不存在 (正确)") << std::endl;

    std::cout << "查找 /home/source: ";
    dir = fs_find("/home/source");
    std::cout << (dir ? "找到 (正确)" : "未找到 (错误)") << std::endl;

    // 验证目录中的文件
    std::cout << "查找 /home/source/file.txt: ";
    FileNode* file = fs_find("/home/source/file.txt");
    std::cout << (file ? "找到 (正确)" : "未找到 (错误)") << std::endl;

    // 尝试将目录移动到自己的子目录 (应该失败)
    std::cout << "\n在 /home/source 创建子目录 subdir: ";
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_EXECUTE;
    perms.other_perms = 0;
    status = fs_create("/home/source", "subdir", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "尝试将 /home/source 移动到自己的子目录 /home/source/subdir (应该失败): ";
    status = fs_move("/home/source", "/home/source/subdir", 0, 0);
    print_fs_status(status);

    // 清理
    std::cout << "\n清理测试目录:" << std::endl;
    std::cout << "删除 /home/source/subdir: ";
    status = fs_delete("/home/source/subdir");
    print_fs_status(status);
    std::cout << "删除 /home/source/file.txt: ";
    status = fs_delete("/home/source/file.txt");
    print_fs_status(status);
    std::cout << "删除 /home/source: ";
    status = fs_delete("/home/source");
    print_fs_status(status);
}

// 测试15: 权限限制的移动和重命名
void test_move_rename_permissions() {
    std::cout << "\n=== 测试15: 权限限制的移动和重命名 ===" << std::endl;

    FileSystemStatus status;
    FileNode* file;
    Permissions perms;

    // alice 创建一个文件
    std::cout << "alice 创建 /alice/private.txt (权限 rwx------): ";
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = 0;
    perms.other_perms = 0;
    status = fs_create("/alice", "private.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // bob 尝试重命名 alice 的文件 (应该失败)
    std::cout << "bob 尝试重命名 /alice/private.txt (应该失败): ";
    status = fs_rename("/alice/private.txt", "public.txt", 1002, 100);
    print_fs_status(status);

    // bob 尝试移动 alice 的文件 (应该失败)
    std::cout << "bob 尝试移动 /alice/private.txt 到 /home (应该失败): ";
    status = fs_move("/alice/private.txt", "/home", 1002, 100);
    print_fs_status(status);

    // alice 自己可以重命名
    std::cout << "\nalice 重命名自己的文件 private.txt 为 mydata.txt: ";
    status = fs_rename("/alice/private.txt", "mydata.txt", 1001, 100);
    print_fs_status(status);

    // 验证重命名成功
    std::cout << "查找 /alice/private.txt: ";
    file = fs_find("/alice/private.txt");
    std::cout << (file ? "仍存在 (错误)" : "已不存在 (正确)") << std::endl;

    std::cout << "查找 /alice/mydata.txt: ";
    file = fs_find("/alice/mydata.txt");
    std::cout << (file ? "找到 (正确)" : "未找到 (错误)") << std::endl;

    // root 可以操作任何文件
    std::cout << "\nroot 重命名 alice 的文件 mydata.txt 为 root_data.txt: ";
    status = fs_rename("/alice/mydata.txt", "root_data.txt", 0, 0);
    print_fs_status(status);

    std::cout << "验证: 查找 /alice/root_data.txt: ";
    file = fs_find("/alice/root_data.txt");
    std::cout << (file ? "找到" : "未找到") << std::endl;

    // 清理
    std::cout << "\n清理测试文件:" << std::endl;
    std::cout << "删除 /alice/root_data.txt: ";
    status = fs_delete("/alice/root_data.txt");
    print_fs_status(status);
}

// 测试16: 文件复制
void test_copy() {
    std::cout << "\n=== 测试16: 文件复制 ===" << std::endl;

    FileSystemStatus status;
    FileNode* file;
    Permissions perms;

    // 创建测试文件
    std::cout << "创建源文件 /home/original.txt (所有者 alice, 权限 rw-r--r--): ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = PERM_READ;
    status = fs_create("/home", "original.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // root 复制文件到 /alice 目录
    std::cout << "\nroot 复制 /home/original.txt 到 /alice: ";
    status = fs_copy("/home/original.txt", "/alice", 0, 0);
    print_fs_status(status);

    // 验证复制成功
    std::cout << "查找 /home/original.txt (源文件): ";
    file = fs_find("/home/original.txt");
    std::cout << (file ? "存在 (正确)" : "不存在 (错误)") << std::endl;

    std::cout << "查找 /alice/original.txt (副本): ";
    file = fs_find("/alice/original.txt");
    if (file) {
        std::cout << "存在 (正确)" << std::endl;
        std::cout << "  所有者: UID=" << file->uid << " (应该是 root UID=0)" << std::endl;
        std::cout << "  权限: ";
        print_permissions(file->perms);
        std::cout << " (继承源文件)" << std::endl;
    } else {
        std::cout << "不存在 (错误)" << std::endl;
    }

    // alice 复制文件到同一目录（重命名效果）
    std::cout << "\nalice 复制 /home/original.txt 到 /home (创建副本): ";
    status = fs_copy("/home/original.txt", "/home", 1001, 100);
    print_fs_status(status);

    std::cout << "查找 /home/original.txt: ";
    file = fs_find("/home/original.txt");
    std::cout << (file ? "存在" : "不存在") << std::endl;

    // 尝试复制到目标位置已存在同名文件 (应该失败)
    std::cout << "\n再次尝试 root 复制 /home/original.txt 到 /alice (同名文件已存在，应该失败): ";
    status = fs_copy("/home/original.txt", "/alice", 0, 0);
    print_fs_status(status);

    // 尝试复制不存在的文件 (应该失败)
    std::cout << "\n尝试复制不存在的文件 /nonexistent.txt (应该失败): ";
    status = fs_copy("/nonexistent.txt", "/alice", 0, 0);
    print_fs_status(status);

    // 尝试复制到不存在的目录 (应该失败)
    std::cout << "\n尝试复制 /home/original.txt 到不存在的目录 /nonexistent (应该失败): ";
    status = fs_copy("/home/original.txt", "/nonexistent", 0, 0);
    print_fs_status(status);

    // 尝试复制目录 (应该失败，只能复制文件)
    std::cout << "\n创建测试目录 /testdir: ";
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_EXECUTE;
    perms.other_perms = 0;
    status = fs_create("/", "testdir", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "尝试复制目录 /testdir (应该失败): ";
    status = fs_copy("/testdir", "/alice", 0, 0);
    print_fs_status(status);

    // 尝试复制到非目录 (应该失败)
    std::cout << "\n尝试复制 /home/original.txt 到文件路径 /alice/original.txt (应该失败): ";
    status = fs_copy("/home/original.txt", "/alice/original.txt", 0, 0);
    print_fs_status(status);

    // 权限测试：尝试复制无读权限的文件
    std::cout << "\n创建无读权限的文件 /home/noread.txt (权限 ---): ";
    perms.owner_perms = 0;  // 无权限
    perms.group_perms = 0;
    perms.other_perms = 0;
    status = fs_create("/home", "noread.txt", 1001, 100, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "bob 尝试复制无读权限的 /home/noread.txt (应该失败): ";
    status = fs_copy("/home/noread.txt", "/alice", 1002, 100);
    print_fs_status(status);

    // 权限测试：目标目录无写权限
    std::cout << "\n创建目录 /readonly (权限 r-xr-xr-x, 所有者 alice, 无写权限): ";
    perms.owner_perms = PERM_READ | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_EXECUTE;
    perms.other_perms = PERM_READ | PERM_EXECUTE;
    status = fs_create("/", "readonly", 1001, 100, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "alice 尝试复制 /home/original.txt 到自己创建的无写权限的 /readonly (应该失败): ";
    status = fs_copy("/home/original.txt", "/readonly", 1001, 100);
    print_fs_status(status);

    // 清理
    std::cout << "\n清理测试文件:" << std::endl;
    std::cout << "删除 /home/original.txt: ";
    status = fs_delete("/home/original.txt");
    print_fs_status(status);
    std::cout << "删除 /alice/original.txt: ";
    status = fs_delete("/alice/original.txt");
    print_fs_status(status);
    std::cout << "删除 /home/noread.txt: ";
    status = fs_delete("/home/noread.txt");
    print_fs_status(status);
    std::cout << "删除 /testdir: ";
    status = fs_delete("/testdir");
    print_fs_status(status);
    // /readonly 目录可能包含文件，需要先用 root 删除其中的文件
    std::cout << "删除 /readonly/original.txt (如果存在): ";
    file = fs_find("/readonly/original.txt");
    if (file) {
        status = fs_delete("/readonly/original.txt");
        print_fs_status(status);
    } else {
        std::cout << "文件不存在，跳过" << std::endl;
    }
    std::cout << "删除 /readonly: ";
    status = fs_delete("/readonly");
    print_fs_status(status);
}

void test_delete_recursive() {
    std::cout << "\n=== 测试17: 递归删除目录 ===" << std::endl;

    FileSystemStatus status;
    FileNode* file;
    Permissions perms;

    // 创建测试目录结构
    std::cout << "创建测试目录结构:" << std::endl;

    // /tree/
    //   ├── file1.txt
    //   ├── file2.txt
    //   └── subtree/
    //       ├── file3.txt
    //       └── file4.txt

    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_EXECUTE;
    perms.other_perms = 0;

    std::cout << "  创建目录 /tree: ";
    status = fs_create("/", "tree", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "  创建 /tree/file1.txt: ";
    status = fs_create("/tree", "file1.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "  创建 /tree/file2.txt: ";
    status = fs_create("/tree", "file2.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "  创建子目录 /tree/subtree: ";
    status = fs_create("/tree", "subtree", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "  创建 /tree/subtree/file3.txt: ";
    status = fs_create("/tree/subtree", "file3.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "  创建 /tree/subtree/file4.txt: ";
    status = fs_create("/tree/subtree", "file4.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // 验证目录结构
    std::cout << "\n验证目录结构:" << std::endl;
    std::cout << "  查找 /tree: ";
    file = fs_find("/tree");
    std::cout << (file ? "存在" : "不存在") << std::endl;

    std::cout << "  查找 /tree/file1.txt: ";
    file = fs_find("/tree/file1.txt");
    std::cout << (file ? "存在" : "不存在") << std::endl;

    std::cout << "  查找 /tree/subtree: ";
    file = fs_find("/tree/subtree");
    std::cout << (file ? "存在" : "不存在") << std::endl;

    std::cout << "  查找 /tree/subtree/file3.txt: ";
    file = fs_find("/tree/subtree/file3.txt");
    std::cout << (file ? "存在" : "不存在") << std::endl;

    // 尝试用普通 fs_delete 删除非空目录 (应该失败)
    std::cout << "\n尝试用普通 fs_delete 删除非空目录 /tree (应该失败): ";
    status = fs_delete("/tree");
    if (status == FS_ERR_NOT_EMPTY) {
        std::cout << "失败: 目录非空 (正确)" << std::endl;
    } else {
        std::cout << "意外结果: ";
        print_fs_status(status);
    }

    // 使用 fs_delete_recursive 删除整个目录树
    std::cout << "\n使用 fs_delete_recursive 递归删除 /tree: ";
    status = fs_delete_recursive("/tree");
    print_fs_status(status);

    // 验证所有文件都被删除
    std::cout << "\n验证删除结果:" << std::endl;
    std::cout << "  查找 /tree: ";
    file = fs_find("/tree");
    std::cout << (file ? "仍存在 (错误)" : "不存在 (正确)") << std::endl;

    std::cout << "  查找 /tree/file1.txt: ";
    file = fs_find("/tree/file1.txt");
    std::cout << (file ? "仍存在 (错误)" : "不存在 (正确)") << std::endl;

    std::cout << "  查找 /tree/subtree: ";
    file = fs_find("/tree/subtree");
    std::cout << (file ? "仍存在 (错误)" : "不存在 (正确)") << std::endl;

    std::cout << "  查找 /tree/subtree/file3.txt: ";
    file = fs_find("/tree/subtree/file3.txt");
    std::cout << (file ? "仍存在 (错误)" : "不存在 (正确)") << std::endl;

    // 测试更深层级的目录结构
    std::cout << "\n\n创建更深层级的目录结构进行测试:" << std::endl;

    // /deep/
    //   ├── level1/
    //   │   ├── level2/
    //   │   │   ├── level3/
    //   │   │   │   └── file.txt
    //   │   │   └── file2.txt
    //   │   └── file1.txt
    //   └── rootfile.txt

    std::cout << "  创建 /deep: ";
    status = fs_create("/", "deep", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "  创建 /deep/level1: ";
    status = fs_create("/deep", "level1", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "  创建 /deep/level1/level2: ";
    status = fs_create("/deep/level1", "level2", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "  创建 /deep/level1/level2/level3: ";
    status = fs_create("/deep/level1/level2", "level3", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "  创建 /deep/level1/level2/level3/file.txt: ";
    status = fs_create("/deep/level1/level2/level3", "file.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "  创建 /deep/level1/level2/file2.txt: ";
    status = fs_create("/deep/level1/level2", "file2.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "  创建 /deep/level1/file1.txt: ";
    status = fs_create("/deep/level1", "file1.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "  创建 /deep/rootfile.txt: ";
    status = fs_create("/deep", "rootfile.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // 验证文件数量增加
    int file_count_before = fs_get_file_count();
    std::cout << "\n当前文件系统文件数: " << file_count_before << std::endl;

    // 递归删除
    std::cout << "递归删除 /deep: ";
    status = fs_delete_recursive("/deep");
    print_fs_status(status);

    // 验证文件数量减少
    int file_count_after = fs_get_file_count();
    std::cout << "删除后文件系统文件数: " << file_count_after << std::endl;

    // 测试删除单个文件 (应该也能工作)
    std::cout << "\n创建单个文件 /singlefile.txt: ";
    status = fs_create("/", "singlefile.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "用 fs_delete_recursive 删除单个文件: ";
    status = fs_delete_recursive("/singlefile.txt");
    print_fs_status(status);

    std::cout << "查找 /singlefile.txt: ";
    file = fs_find("/singlefile.txt");
    std::cout << (file ? "仍存在 (错误)" : "不存在 (正确)") << std::endl;

    // 测试删除空目录
    std::cout << "\n创建空目录 /emptydir: ";
    status = fs_create("/", "emptydir", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "用 fs_delete_recursive 删除空目录: ";
    status = fs_delete_recursive("/emptydir");
    print_fs_status(status);

    std::cout << "查找 /emptydir: ";
    file = fs_find("/emptydir");
    std::cout << (file ? "仍存在 (错误)" : "不存在 (正确)") << std::endl;

    // 尝试删除根目录 (应该失败)
    std::cout << "\n尝试删除根目录 / (应该失败): ";
    status = fs_delete_recursive("/");
    if (status == FS_ERR_INVALID) {
        std::cout << "失败: 不能删除根目录 (正确)" << std::endl;
    } else {
        std::cout << "意外结果: ";
        print_fs_status(status);
    }

    // 尝试删除不存在的路径 (应该失败)
    std::cout << "\n尝试删除不存在的路径 /nonexistent (应该失败): ";
    status = fs_delete_recursive("/nonexistent");
    if (status == FS_ERR_NOT_FOUND) {
        std::cout << "失败: 路径不存在 (正确)" << std::endl;
    } else {
        std::cout << "意外结果: ";
        print_fs_status(status);
    }
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

    // 第三阶段: 文件移动和重命名测试
    std::cout << "\n############ 第三阶段: 文件移动和重命名测试 ############" << std::endl;
    test_rename();
    test_rename_directory();
    test_move_file();
    test_move_directory();
    test_move_rename_permissions();
    test_copy();
    test_delete_recursive();

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
