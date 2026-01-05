// DemoProject.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../KernProject/user_manager.h"
#include "../KernProject/file_system.h"
#include <windows.h>

// ==================== 字符串辅助函数 ====================

// 字符串长度(带最大长度限制,防止缓冲区溢出)
static int str_length(const char* str) {
    if (str == 0) return 0;
    int len = 0;
    int max_len = MAX_PATH_LEN * 2;  // 防止无限循环
    while (str[len] != '\0' && len < max_len) len++;
    return len;
}

// 字符串比较(带最大长度限制)
static int str_compare(const char* s1, const char* s2) {
    if (s1 == 0 || s2 == 0) return -1;
    int i = 0;
    int max_cmp = MAX_PATH_LEN;  // 限制比较长度
    while (s1[i] != '\0' && s2[i] != '\0' && i < max_cmp) {
        if (s1[i] != s2[i]) return s1[i] - s2[i];
        i++;
    }
    return s1[i] - s2[i];
}

// 字符串复制(带缓冲区大小检查)
static void str_copy(char* dest, const char* src, int max_len = MAX_PATH_LEN) {
    if (dest == 0 || src == 0) return;
    int i = 0;
    while (src[i] != '\0' && i < max_len - 1) {  // 保留空间给 '\0'
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';  // 确保以 null 结尾
}

// 字符串拼接(带缓冲区大小检查)
static void str_concat(char* dest, const char* src, int max_len = MAX_PATH_LEN) {
    if (dest == 0 || src == 0) return;
    int i = 0;
    while (dest[i] != '\0' && i < max_len) i++;  // 找到末尾
    int j = 0;
    while (src[j] != '\0' && i < max_len - 1) {  // 保留空间给 '\0'
        dest[i++] = src[j++];
    }
    dest[i] = '\0';  // 确保以 null 结尾
}

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

void test_normalize_path() {
    std::cout << "\n=== 测试18: 路径规范化功能 ===" << std::endl;

    char output[MAX_PATH_LEN];
    int result;

    // 测试1: 处理 .
    std::cout << "\n测试1: 处理 . (当前目录)" << std::endl;
    result = fs_normalize_path("/home/./user", output);
    std::cout << "  /home/./user -> \"" << output << "\" (预期: /home/user)" << std::endl;
    std::cout << "  结果: " << (result == 0 ? "成功" : "失败") << std::endl;

    // 测试2: 处理 ..
    std::cout << "\n测试2: 处理 .. (父目录)" << std::endl;
    result = fs_normalize_path("/home/../user", output);
    std::cout << "  /home/../user -> \"" << output << "\" (预期: /user)" << std::endl;
    std::cout << "  结果: " << (result == 0 ? "成功" : "失败") << std::endl;

    // 测试3: 处理多个 ..
    std::cout << "\n测试3: 处理多个 .." << std::endl;
    result = fs_normalize_path("/a/b/c/../../d", output);
    std::cout << "  /a/b/c/../../d -> \"" << output << "\" (预期: /a/d)" << std::endl;
    std::cout << "  结果: " << (result == 0 ? "成功" : "失败") << std::endl;

    // 测试4: 处理连续的 /
    std::cout << "\n测试4: 处理连续的 /" << std::endl;
    result = fs_normalize_path("/home///user", output);
    std::cout << "  /home///user -> \"" << output << "\" (预期: /home/user)" << std::endl;
    std::cout << "  结果: " << (result == 0 ? "成功" : "失败") << std::endl;

    // 测试5: 处理混合的 . 和 ..
    std::cout << "\n测试5: 处理混合的 . 和 .." << std::endl;
    result = fs_normalize_path("/home/./user/../admin/.", output);
    std::cout << "  /home/./user/../admin/. -> \"" << output << "\" (预期: /home/admin)" << std::endl;
    std::cout << "  结果: " << (result == 0 ? "成功" : "失败") << std::endl;

    // 测试6: 根目录的特殊情况
    std::cout << "\n测试6: 根目录的 .. 不能超出根目录" << std::endl;
    result = fs_normalize_path("/../..", output);
    std::cout << "  /../.. -> \"" << output << "\" (预期: /)" << std::endl;
    std::cout << "  结果: " << (result == 0 ? "成功" : "失败") << std::endl;

    // 测试7: 移除末尾的 /
    std::cout << "\n测试7: 移除末尾的 /" << std::endl;
    result = fs_normalize_path("/home/user/", output);
    std::cout << "  /home/user/ -> \"" << output << "\" (预期: /home/user)" << std::endl;
    std::cout << "  结果: " << (result == 0 ? "成功" : "失败") << std::endl;

    // 测试8: 相对路径
    std::cout << "\n测试8: 相对路径规范化" << std::endl;
    result = fs_normalize_path("./user", output);
    std::cout << "  ./user -> \"" << output << "\" (预期: user)" << std::endl;
    std::cout << "  结果: " << (result == 0 ? "成功" : "失败") << std::endl;

    // 测试9: 相对路径中的 ..
    result = fs_normalize_path("../home", output);
    std::cout << "  ../home -> \"" << output << "\" (预期: ../home)" << std::endl;
    std::cout << "  结果: " << (result == 0 ? "成功" : "失败") << std::endl;

    // 测试10: 复杂路径
    std::cout << "\n测试10: 复杂路径" << std::endl;
    result = fs_normalize_path("/a/b/../c/./d/e/../f", output);
    std::cout << "  /a/b/../c/./d/e/../f -> \"" << output << "\" (预期: /a/c/d/f)" << std::endl;
    std::cout << "  结果: " << (result == 0 ? "成功" : "失败") << std::endl;
}

void test_find_relative() {
    std::cout << "\n=== 测试19: 相对路径查找功能 ===" << std::endl;

    FileSystemStatus status;
    FileNode* file;
    FileNode* current;
    Permissions perms;

    // 创建测试目录结构
    std::cout << "\n创建测试目录结构:" << std::endl;
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_EXECUTE;
    perms.other_perms = 0;

    std::cout << "  创建 /home: ";
    status = fs_create("/", "home", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "  创建 /home/user: ";
    status = fs_create("/home", "user", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "  创建 /home/user/file.txt: ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = 0;
    status = fs_create("/home/user", "file.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "  创建 /home/admin: ";
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_EXECUTE;
    perms.other_perms = 0;
    status = fs_create("/home", "admin", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    // 测试1: 从 /home 查找相对路径 ./user
    std::cout << "\n测试1: 从 /home 查找 ./user" << std::endl;
    current = fs_find("/home");
    std::cout << "  当前目录: /home" << std::endl;
    file = fs_find_relative(current, "./user");
    std::cout << "  查找 ./user: " << (file ? "找到" : "未找到") << std::endl;

    // 测试2: 从 /home/user 查找 ../admin
    std::cout << "\n测试2: 从 /home/user 查找 ../admin" << std::endl;
    current = fs_find("/home/user");
    std::cout << "  当前目录: /home/user" << std::endl;
    file = fs_find_relative(current, "../admin");
    std::cout << "  查找 ../admin: " << (file ? "找到" : "未找到") << std::endl;

    // 测试3: 从 /home/user 查找 ./file.txt
    std::cout << "\n测试3: 从 /home/user 查找 ./file.txt" << std::endl;
    current = fs_find("/home/user");
    std::cout << "  当前目录: /home/user" << std::endl;
    file = fs_find_relative(current, "./file.txt");
    std::cout << "  查找 ./file.txt: " << (file ? "找到" : "未找到") << std::endl;

    // 测试4: 从 /home/user 查找 ../admin/../user/file.txt
    std::cout << "\n测试4: 从 /home/user 查找 ../admin/../user/file.txt" << std::endl;
    current = fs_find("/home/user");
    std::cout << "  当前目录: /home/user" << std::endl;
    file = fs_find_relative(current, "../admin/../user/file.txt");
    std::cout << "  查找 ../admin/../user/file.txt: " << (file ? "找到" : "未找到") << std::endl;

    // 测试5: 使用绝对路径（应忽略当前目录）
    std::cout << "\n测试5: 使用绝对路径 /home/admin" << std::endl;
    current = fs_find("/home/user");
    std::cout << "  当前目录: /home/user" << std::endl;
    file = fs_find_relative(current, "/home/admin");
    std::cout << "  查找 /home/admin: " << (file ? "找到" : "未找到") << std::endl;

    // 测试6: 多层 ..
    std::cout << "\n测试6: 从 /home/user 查找 ../../.." << std::endl;
    current = fs_find("/home/user");
    std::cout << "  当前目录: /home/user" << std::endl;
    FileNode* root = fs_find("/");
    file = fs_find_relative(current, "../../..");
    std::cout << "  查找 ../../.. (应到达根目录): " << (file ? "找到" : "未找到") << std::endl;
    if (file) {
        std::cout << "  结果是否为根目录: " << (file == root ? "是" : "否") << std::endl;
    }

    // 测试7: 查找不存在的路径
    std::cout << "\n测试7: 查找不存在的路径" << std::endl;
    current = fs_find("/home");
    std::cout << "  当前目录: /home" << std::endl;
    file = fs_find_relative(current, "./nonexistent");
    std::cout << "  查找 ./nonexistent: " << (file ? "找到(错误)" : "未找到(正确)") << std::endl;

    // 清理
    std::cout << "\n清理测试文件:" << std::endl;
    std::cout << "删除 /home/user/file.txt: ";
    status = fs_delete("/home/user/file.txt");
    print_fs_status(status);
    std::cout << "删除 /home/user: ";
    status = fs_delete("/home/user");
    print_fs_status(status);
    std::cout << "删除 /home/admin: ";
    status = fs_delete("/home/admin");
    print_fs_status(status);
    std::cout << "删除 /home: ";
    status = fs_delete("/home");
    print_fs_status(status);
}

// ==================== 任务六: 批量测试脚本 ====================

/**
 * 场景一: 多用户协作项目测试
 * 模拟真实开发团队协作场景
 */
void test_comprehensive_scenario() {
    FileSystemStatus status;
    Permissions perms;
    FileNode* node;

    std::cout << "\n========================================" << std::endl;
    std::cout << "场景一: 多用户协作项目测试" << std::endl;
    std::cout << "========================================" << std::endl;

    // 步骤1: root创建项目目录结构
    std::cout << "\n[步骤1] root创建项目目录结构" << std::endl;
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.other_perms = PERM_READ | PERM_EXECUTE;

    std::cout << "创建 /project 目录: ";
    status = fs_create("/", "project", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "创建 /project/src 目录: ";
    status = fs_create("/project", "src", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "创建 /project/docs 目录: ";
    status = fs_create("/project", "docs", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    // 步骤2: 创建开发团队
    std::cout << "\n[步骤2] 创建开发团队用户和组" << std::endl;
    std::cout << "创建用户 alice (UID=1): ";
    um_create_user("alice", 1, 1);
    std::cout << "成功" << std::endl;

    std::cout << "创建用户 bob (UID=2): ";
    um_create_user("bob", 2, 2);
    std::cout << "成功" << std::endl;

    std::cout << "创建用户 charlie (UID=3): ";
    um_create_user("charlie", 3, 3);
    std::cout << "成功" << std::endl;

    std::cout << "创建组 developers (GID=10): ";
    um_create_group("developers", 10);
    std::cout << "成功" << std::endl;

    std::cout << "添加alice到developers组: ";
    um_add_group_member(10, 1);
    std::cout << "成功" << std::endl;

    std::cout << "添加bob到developers组: ";
    um_add_group_member(10, 2);
    std::cout << "成功" << std::endl;

    // 步骤3: alice创建项目文件
    std::cout << "\n[步骤3] alice创建项目文件" << std::endl;
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_WRITE;
    perms.other_perms = PERM_READ;

    std::cout << "alice创建 /project/src/main.c: ";
    status = fs_create("/project/src", "main.c", 1, 10, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "alice创建 /project/docs/readme.md: ";
    status = fs_create("/project/docs", "readme.md", 1, 10, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // 步骤4: bob修改文件(测试组写权限)
    std::cout << "\n[步骤4] bob尝试修改文件" << std::endl;
    node = fs_find("/project/src/main.c");

    std::cout << "bob检查对main.c的写权限: ";
    PermissionResult perm_result = fs_check_permission(node, 2, 10, OP_WRITE);
    std::cout << (perm_result == PERM_GRANTED ? "允许" : "拒绝") << std::endl;

    std::cout << "bob重命名main.c为core.c: ";
    status = fs_rename("/project/src/main.c", "core.c", 2, 10);
    print_fs_status(status);

    // 步骤5: charlie尝试访问(无组成员权限)
    std::cout << "\n[步骤5] charlie尝试访问文件" << std::endl;
    node = fs_find("/project/src/core.c");

    std::cout << "charlie检查对core.c的读权限: ";
    perm_result = fs_check_permission(node, 3, 3, OP_READ);
    std::cout << (perm_result == PERM_GRANTED ? "允许" : "拒绝") << std::endl;

    std::cout << "charlie尝试删除core.c: ";
    status = fs_delete("/project/src/core.c");
    if (status == FS_ERR_PERMISSION) {
        std::cout << "拒绝(正确)" << std::endl;
    } else {
        std::cout << "成功(错误!应该被拒绝)" << std::endl;
    }

    // 步骤6: 测试目录权限
    std::cout << "\n[步骤6] 测试目录权限控制" << std::endl;
    std::cout << "charlie尝试在/project/src创建文件: ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = PERM_READ;
    status = fs_create("/project/src", "test.c", 3, 3, perms, FILE_TYPE_REGULAR);
    if (status == FS_ERR_PERMISSION) {
        std::cout << "拒绝(正确)" << std::endl;
    } else {
        std::cout << "成功(错误!)" << std::endl;
    }

    // 清理
    std::cout << "\n[清理] 删除测试数据" << std::endl;
    std::cout << "递归删除 /project: ";
    status = fs_delete_recursive("/project");
    print_fs_status(status);

    um_delete_user(1);
    um_delete_user(2);
    um_delete_user(3);
    um_delete_group(10);

    std::cout << "\n✅ 场景一测试完成!" << std::endl;
}

/**
 * 场景二: 权限继承测试
 * 验证不同层级的访问权限
 */
void test_multi_user_collaboration() {
    FileSystemStatus status;
    Permissions perms;
    FileNode* node;

    std::cout << "\n========================================" << std::endl;
    std::cout << "场景二: 权限继承测试" << std::endl;
    std::cout << "========================================" << std::endl;

    // 创建测试用户
    um_create_user("user1", 100, 100);
    um_create_user("user2", 200, 200);

    // 步骤1: 创建层级目录结构
    std::cout << "\n[步骤1] 创建层级目录结构" << std::endl;

    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_EXECUTE;
    perms.other_perms = PERM_READ | PERM_EXECUTE;

    std::cout << "创建 /test (权限755): ";
    status = fs_create("/", "test", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "创建 /test/level1: ";
    status = fs_create("/test", "level1", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    std::cout << "创建 /test/level1/level2: ";
    status = fs_create("/test/level1", "level2", 0, 0, perms, FILE_TYPE_DIRECTORY);
    print_fs_status(status);

    // 步骤2: 在不同层级创建文件
    std::cout << "\n[步骤2] 在不同层级创建文件" << std::endl;

    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = PERM_READ;

    std::cout << "创建 /test/file1.txt (权限644): ";
    status = fs_create("/test", "file1.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "创建 /test/level1/file2.txt (权限600): ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = 0;
    perms.other_perms = 0;
    status = fs_create("/test/level1", "file2.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    // 步骤3: 测试user1的访问权限
    std::cout << "\n[步骤3] 测试user1的访问权限" << std::endl;

    node = fs_find("/test/file1.txt");
    std::cout << "user1读取 /test/file1.txt: ";
    PermissionResult result = fs_check_permission(node, 100, 100, OP_READ);
    std::cout << (result == PERM_GRANTED ? "允许" : "拒绝") << std::endl;

    node = fs_find("/test/level1/file2.txt");
    std::cout << "user1读取 /test/level1/file2.txt: ";
    result = fs_check_permission(node, 100, 100, OP_READ);
    std::cout << (result == PERM_GRANTED ? "允许" : "拒绝") << std::endl;

    // 步骤4: 修改目录权限并测试影响
    std::cout << "\n[步骤4] 修改目录权限并测试" << std::endl;

    std::cout << "将 /test/level1 权限改为 700: ";
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = 0;
    perms.other_perms = 0;
    status = fs_chmod("/test/level1", perms);
    print_fs_status(status);

    node = fs_find("/test/level1");
    std::cout << "user1访问 /test/level1 目录: ";
    result = fs_check_permission(node, 100, 100, OP_EXECUTE);
    std::cout << (result == PERM_GRANTED ? "允许" : "拒绝") << std::endl;

    // 步骤5: 测试chmod的影响
    std::cout << "\n[步骤5] 测试chmod对文件权限的影响" << std::endl;

    std::cout << "将 /test/file1.txt 权限改为 000: ";
    perms.owner_perms = 0;
    perms.group_perms = 0;
    perms.other_perms = 0;
    status = fs_chmod("/test/file1.txt", perms);
    print_fs_status(status);

    node = fs_find("/test/file1.txt");
    std::cout << "root读取 file1.txt: ";
    result = fs_check_permission(node, 0, 0, OP_READ);
    std::cout << (result == PERM_GRANTED ? "允许(root始终可访问)" : "拒绝") << std::endl;

    // 清理
    std::cout << "\n[清理] 删除测试数据" << std::endl;
    std::cout << "递归删除 /test: ";
    status = fs_delete_recursive("/test");
    print_fs_status(status);

    um_delete_user(100);
    um_delete_user(200);

    std::cout << "\n✅ 场景二测试完成!" << std::endl;
}

/**
 * 场景三: 边界条件测试
 * 测试系统的极限情况
 */
void test_edge_cases() {
    FileSystemStatus status;
    Permissions perms;
    int file_count = 0;
    int success_count = 0;
    int fail_count = 0;

    std::cout << "\n========================================" << std::endl;
    std::cout << "场景三: 边界条件测试" << std::endl;
    std::cout << "========================================" << std::endl;

    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_EXECUTE;
    perms.other_perms = PERM_READ | PERM_EXECUTE;

    // 测试1: 创建深层嵌套目录
    std::cout << "\n[测试1] 创建深层嵌套目录结构" << std::endl;
    status = fs_create("/", "deep", 0, 0, perms, FILE_TYPE_DIRECTORY);
    if (status == FS_SUCCESS) success_count++; else fail_count++;

    status = fs_create("/deep", "level1", 0, 0, perms, FILE_TYPE_DIRECTORY);
    if (status == FS_SUCCESS) success_count++; else fail_count++;

    status = fs_create("/deep/level1", "level2", 0, 0, perms, FILE_TYPE_DIRECTORY);
    if (status == FS_SUCCESS) success_count++; else fail_count++;

    status = fs_create("/deep/level1/level2", "level3", 0, 0, perms, FILE_TYPE_DIRECTORY);
    if (status == FS_SUCCESS) success_count++; else fail_count++;

    status = fs_create("/deep/level1/level2/level3", "level4", 0, 0, perms, FILE_TYPE_DIRECTORY);
    if (status == FS_SUCCESS) success_count++; else fail_count++;

    std::cout << "创建了5层嵌套目录" << std::endl;
    std::cout << "验证: 查找 /deep/level1/level2/level3/level4: ";
    FileNode* node = fs_find("/deep/level1/level2/level3/level4");
    std::cout << (node ? "成功" : "失败") << std::endl;

    // 测试2: 在单个目录中创建多个文件
    std::cout << "\n[测试2] 在单个目录中创建多个文件" << std::endl;
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = PERM_READ;

    for (int i = 0; i < 10; i++) {
        char filename[32];
        str_copy(filename, "file");
        // 简单的数字转字符串
        if (i < 10) {
            filename[4] = '0' + i;
            filename[5] = '\0';
        }

        status = fs_create("/deep/level1/level2/level3", filename, 0, 0, perms, FILE_TYPE_REGULAR);
        if (status == FS_SUCCESS) {
            file_count++;
            success_count++;
        } else {
            fail_count++;
        }
    }
    std::cout << "成功创建 " << file_count << " 个文件" << std::endl;

    // 测试3: 超长文件名测试
    std::cout << "\n[测试3] 超长文件名测试" << std::endl;
    char long_name[MAX_FILENAME_LEN + 10];
    for (int i = 0; i < MAX_FILENAME_LEN + 5; i++) {
        long_name[i] = 'a';
    }
    long_name[MAX_FILENAME_LEN + 5] = '\0';

    std::cout << "尝试创建超长文件名(" << (MAX_FILENAME_LEN + 5) << "字符): ";
    status = fs_create("/", long_name, 0, 0, perms, FILE_TYPE_REGULAR);
    if (status == FS_ERR_INVALID) {
        std::cout << "正确拒绝" << std::endl;
        success_count++;
    } else {
        std::cout << "错误!应该拒绝" << std::endl;
        fail_count++;
    }

    // 测试4: 最大长度文件名
    std::cout << "\n[测试4] 最大长度文件名测试(" << (MAX_FILENAME_LEN - 1) << "字符)" << std::endl;
    char max_name[MAX_FILENAME_LEN];
    for (int i = 0; i < MAX_FILENAME_LEN - 1; i++) {
        max_name[i] = 'b';
    }
    max_name[MAX_FILENAME_LEN - 1] = '\0';

    std::cout << "创建最大长度文件名: ";
    status = fs_create("/", max_name, 0, 0, perms, FILE_TYPE_REGULAR);
    if (status == FS_SUCCESS) {
        std::cout << "成功" << std::endl;
        success_count++;
        // 验证可以找到
        node = fs_find("/");
        FileNode* child = node->children;
        int found = 0;
        while (child != 0) {
            if (str_compare(child->filename, max_name) == 0) {
                found = 1;
                break;
            }
            child = child->next;
        }
        if (found) {
            std::cout << "验证: 可以找到该文件" << std::endl;
        }
        // 清理
        fs_delete(max_name);
    } else {
        std::cout << "失败" << std::endl;
        fail_count++;
    }

    // 测试5: 删除不存在的文件
    std::cout << "\n[测试5] 删除不存在的文件" << std::endl;
    std::cout << "删除 /nonexistent/file.txt: ";
    status = fs_delete("/nonexistent/file.txt");
    if (status == FS_ERR_NOT_FOUND) {
        std::cout << "正确返回FS_ERR_NOT_FOUND" << std::endl;
        success_count++;
    } else {
        std::cout << "错误状态码" << std::endl;
        fail_count++;
    }

    // 测试6: 移动文件到自身
    std::cout << "\n[测试6] 移动文件到自身目录" << std::endl;
    std::cout << "创建测试文件 /testmove.txt: ";
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = PERM_READ;
    status = fs_create("/", "testmove.txt", 0, 0, perms, FILE_TYPE_REGULAR);
    print_fs_status(status);

    std::cout << "移动到相同目录: ";
    status = fs_move("/testmove.txt", "/", 0, 0);
    if (status == FS_ERR_EXISTS) {
        std::cout << "正确拒绝(文件已存在)" << std::endl;
        success_count++;
    } else {
        std::cout << "状态码: " << status << std::endl;
        fail_count++;
    }

    // 清理
    std::cout << "\n[清理] 删除所有测试数据" << std::endl;
    fs_delete("/testmove.txt");
    fs_delete_recursive("/deep");

    // 统计结果
    std::cout << "\n========================================" << std::endl;
    std::cout << "边界测试统计:" << std::endl;
    std::cout << "  成功: " << success_count << std::endl;
    std::cout << "  失败: " << fail_count << std::endl;
    std::cout << "  成功率: " << (success_count * 100 / (success_count + fail_count)) << "%" << std::endl;
    std::cout << "========================================" << std::endl;

    std::cout << "\n✅ 场景三测试完成!" << std::endl;
}

// ==================== 交互式命令行界面 ====================

// 全局状态变量
static int g_current_uid = 0;       // 当前用户ID (默认root)
static int g_current_gid = 0;       // 当前用户组ID (默认root)
static FileNode* g_current_dir = 0; // 当前目录节点

// 获取用户名
const char* get_username(int uid) {
    // 特殊处理: UID=0 默认为 root
    if (uid == 0) {
        User* user = um_find_user_by_uid(0);
        if (user) {
            return user->username;
        }
        return "root";
    }

    User* user = um_find_user_by_uid(uid);
    if (user) {
        return user->username;
    }
    return "unknown";
}

// 打印权限字符串 (rwxr-xr-x格式)
void print_permission_string(Permissions perms) {
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

// 获取当前目录路径字符串(优化版本: 减少字符串复制)
void get_current_path(char* path, FileNode* node) {
    if (node == 0 || node == fs_get_root()) {
        str_copy(path, "/");
        return;
    }

    // 使用迭代方式构建路径,从叶节点向根节点遍历
    char temp[MAX_PATH_LEN];
    int pos = MAX_PATH_LEN - 1;  // 从缓冲区末尾开始
    temp[pos] = '\0';
    pos--;

    FileNode* current = node;
    while (current != 0 && current != fs_get_root()) {
        // 计算当前文件名长度
        int len = str_length(current->filename);

        // 检查空间是否足够
        if (pos - len < 0) {
            str_copy(path, "/");
            return;
        }

        // 复制文件名(倒序)
        for (int i = len - 1; i >= 0; i--) {
            temp[pos--] = current->filename[i];
        }

        // 添加路径分隔符(除了根目录)
        if (current->parent != 0 && current->parent != fs_get_root()) {
            temp[pos--] = '/';
        }

        current = current->parent;
    }

    // 添加根目录标记
    if (temp[pos + 1] != '/') {
        temp[pos--] = '/';
    }

    // 正向复制到结果缓冲区(只需一次复制)
    int i = 0;
    int j = pos + 1;
    while (temp[j] != '\0') {
        path[i++] = temp[j++];
    }
    path[i] = '\0';
}

// 打印命令提示符
void print_prompt() {
    char path[MAX_PATH_LEN];
    get_current_path(path, g_current_dir);
    std::cout << "[" << get_username(g_current_uid) << "@" << path << "]$ ";
}

// 命令: ls - 列出目录内容
void cmd_ls(const char* path) {
    FileNode* dir;
    FileNode* child;
    char target_path[MAX_PATH_LEN];

    // 确定目标目录
    if (path == 0 || str_length(path) == 0) {
        dir = g_current_dir;
    } else {
        // 解析路径
        if (path[0] == '/') {
            // 绝对路径
            str_copy(target_path, path);
            dir = fs_find(target_path);
        } else {
            // 相对路径
            FileNode* found = fs_find_relative(g_current_dir, path);
            dir = found;
        }
    }

    if (dir == 0) {
        std::cout << "ls: 目录不存在" << std::endl;
        return;
    }

    if (dir->type != FILE_TYPE_DIRECTORY) {
        std::cout << "ls: 不是目录" << std::endl;
        return;
    }

    // 检查读权限
    if (fs_check_permission(dir, g_current_uid, g_current_gid, OP_READ) != PERM_GRANTED) {
        std::cout << "ls: 权限不足" << std::endl;
        return;
    }

    // 列出内容
    child = dir->children;
    if (child == 0) {
        std::cout << "(空目录)" << std::endl;
        return;
    }

    while (child != 0) {
        // 打印文件类型
        std::cout << ((child->type == FILE_TYPE_DIRECTORY) ? "d" : "-");

        // 打印权限
        print_permission_string(child->perms);

        // 打印所有者
        std::cout << " uid=" << child->uid;

        // 打印文件名
        std::cout << " " << child->filename;

        // 如果是目录，添加 /
        if (child->type == FILE_TYPE_DIRECTORY) {
            std::cout << "/";
        }

        std::cout << std::endl;
        child = child->next;
    }
}

// 命令: mkdir - 创建目录
void cmd_mkdir(const char* path) {
    char dirname[MAX_PATH_LEN];
    char filename[MAX_FILENAME_LEN];
    char full_path[MAX_PATH_LEN];
    FileNode* parent_dir;
    Permissions perms;

    if (path == 0 || str_length(path) == 0) {
        std::cout << "mkdir: 缺少参数" << std::endl;
        return;
    }

    // 构建完整路径
    if (path[0] == '/') {
        // 绝对路径,直接使用
        str_copy(full_path, path);
    } else {
        // 相对路径,拼接当前目录
        get_current_path(full_path, g_current_dir);
        if (str_compare(full_path, "/") != 0) {
            str_concat(full_path, "/");
        }
        str_concat(full_path, path);
    }

    // 规范化路径(处理 .、.. 等)
    char normalized_path[MAX_PATH_LEN];
    if (fs_normalize_path(full_path, normalized_path) != 0) {
        std::cout << "mkdir: 无效路径" << std::endl;
        return;
    }

    // 解析路径
    if (fs_parse_path(normalized_path, dirname, filename) != 0) {
        std::cout << "mkdir: 无效路径" << std::endl;
        return;
    }

    // 查找父目录
    char parent_path[MAX_PATH_LEN];
    if (dirname[0] == '\0') {
        // 使用当前目录
        get_current_path(parent_path, g_current_dir);
        parent_dir = g_current_dir;
    } else {
        // 绝对路径
        str_copy(parent_path, dirname);
        parent_dir = fs_find(dirname);
    }

    if (parent_dir == 0 || parent_dir->type != FILE_TYPE_DIRECTORY) {
        std::cout << "mkdir: 父目录不存在" << std::endl;
        return;
    }

    // 检查父目录写权限
    if (fs_check_permission(parent_dir, g_current_uid, g_current_gid, OP_WRITE) != PERM_GRANTED) {
        std::cout << "mkdir: 权限不足" << std::endl;
        return;
    }

    // 设置默认权限
    perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    perms.group_perms = PERM_READ | PERM_EXECUTE;
    perms.other_perms = PERM_READ | PERM_EXECUTE;

    // 创建目录
    FileSystemStatus status = fs_create(parent_path, filename, g_current_uid, g_current_gid, perms, FILE_TYPE_DIRECTORY);
    if (status != FS_SUCCESS) {
        std::cout << "mkdir: 创建失败" << std::endl;
    }
}

// 命令: touch - 创建文件
void cmd_touch(const char* path) {
    char dirname[MAX_PATH_LEN];
    char filename[MAX_FILENAME_LEN];
    char full_path[MAX_PATH_LEN];
    FileNode* parent_dir;
    Permissions perms;

    if (path == 0 || str_length(path) == 0) {
        std::cout << "touch: 缺少参数" << std::endl;
        return;
    }

    // 构建完整路径
    if (path[0] == '/') {
        // 绝对路径,直接使用
        str_copy(full_path, path);
    } else {
        // 相对路径,拼接当前目录
        get_current_path(full_path, g_current_dir);
        if (str_compare(full_path, "/") != 0) {
            str_concat(full_path, "/");
        }
        str_concat(full_path, path);
    }

    // 规范化路径(处理 .、.. 等)
    char normalized_path[MAX_PATH_LEN];
    if (fs_normalize_path(full_path, normalized_path) != 0) {
        std::cout << "touch: 无效路径" << std::endl;
        return;
    }

    // 解析路径
    if (fs_parse_path(normalized_path, dirname, filename) != 0) {
        std::cout << "touch: 无效路径" << std::endl;
        return;
    }

    // 查找父目录
    char parent_path[MAX_PATH_LEN];
    if (dirname[0] == '\0') {
        // 使用当前目录
        get_current_path(parent_path, g_current_dir);
        parent_dir = g_current_dir;
    } else {
        // 绝对路径
        str_copy(parent_path, dirname);
        parent_dir = fs_find(dirname);
    }

    if (parent_dir == 0 || parent_dir->type != FILE_TYPE_DIRECTORY) {
        std::cout << "touch: 父目录不存在" << std::endl;
        return;
    }

    // 检查父目录写权限
    if (fs_check_permission(parent_dir, g_current_uid, g_current_gid, OP_WRITE) != PERM_GRANTED) {
        std::cout << "touch: 权限不足" << std::endl;
        return;
    }

    // 设置默认权限
    perms.owner_perms = PERM_READ | PERM_WRITE;
    perms.group_perms = PERM_READ;
    perms.other_perms = PERM_READ;

    // 创建文件
    FileSystemStatus status = fs_create(parent_path, filename, g_current_uid, g_current_gid, perms, FILE_TYPE_REGULAR);
    if (status != FS_SUCCESS) {
        std::cout << "touch: 创建失败" << std::endl;
    }
}

// 命令: rm - 删除文件或目录
void cmd_rm(const char* path, int recursive) {
    FileNode* file;
    char full_path[MAX_PATH_LEN];

    if (path == 0 || str_length(path) == 0) {
        std::cout << "rm: 缺少参数" << std::endl;
        return;
    }

    // 解析路径
    if (path[0] == '/') {
        str_copy(full_path, path);
    } else {
        FileNode* found = fs_find_relative(g_current_dir, path);
        if (found == 0) {
            std::cout << "rm: 文件不存在" << std::endl;
            return;
        }
        get_current_path(full_path, found);
    }

    file = fs_find(full_path);
    if (file == 0) {
        std::cout << "rm: 文件不存在" << std::endl;
        return;
    }

    // 执行删除
    FileSystemStatus status;
    if (recursive) {
        status = fs_delete_recursive(full_path);
    } else {
        status = fs_delete(full_path);
    }

    if (status != FS_SUCCESS) {
        if (status == FS_ERR_PERMISSION) {
            std::cout << "rm: 权限不足" << std::endl;
        } else if (status == FS_ERR_NOT_EMPTY) {
            std::cout << "rm: 目录非空，使用 rm -r 递归删除" << std::endl;
        } else {
            std::cout << "rm: 删除失败" << std::endl;
        }
    }
}

// 命令: mv - 移动/重命名文件
void cmd_mv(const char* src_path, const char* dest_path) {
    char src_full[MAX_PATH_LEN];
    char dest_dirname[MAX_PATH_LEN];
    char dest_filename[MAX_FILENAME_LEN];
    char temp_path[MAX_PATH_LEN];
    FileNode* src_file;
    FileNode* dest_node;
    FileNode* dest_parent;

    if (src_path == 0 || dest_path == 0 ||
        str_length(src_path) == 0 || str_length(dest_path) == 0) {
        std::cout << "mv: 缺少参数" << std::endl;
        return;
    }

    // 解析源路径
    if (src_path[0] == '/') {
        str_copy(src_full, src_path);
    } else {
        src_file = fs_find_relative(g_current_dir, src_path);
        if (src_file == 0) {
            std::cout << "mv: 源文件不存在" << std::endl;
            return;
        }
        get_current_path(src_full, src_file);
    }

    // 检查源文件
    src_file = fs_find(src_full);
    if (src_file == 0) {
        std::cout << "mv: 源文件不存在" << std::endl;
        return;
    }

    // 解析目标路径
    // 首先尝试查找目标是否存在
    if (dest_path[0] == '/') {
        dest_node = fs_find(dest_path);
    } else {
        dest_node = fs_find_relative(g_current_dir, dest_path);
    }

    if (dest_node != 0 && dest_node->type == FILE_TYPE_DIRECTORY) {
        // 目标是目录,移动到该目录中,保持原文件名
        str_copy(dest_dirname, dest_path);
        str_copy(dest_filename, src_file->filename);
    } else {
        // 目标不是目录或不存在,解析为目录+文件名
        if (fs_parse_path(dest_path, dest_dirname, dest_filename) != 0) {
            std::cout << "mv: 目标路径无效" << std::endl;
            return;
        }

        // 如果文件名为空,使用源文件名
        if (str_length(dest_filename) == 0) {
            str_copy(dest_filename, src_file->filename);
        }
    }

    // 解析目标目录
    if (dest_dirname[0] == '\0') {
        dest_parent = g_current_dir;
    } else if (dest_dirname[0] == '/') {
        dest_parent = fs_find(dest_dirname);
    } else {
        dest_parent = fs_find_relative(g_current_dir, dest_dirname);
    }

    if (dest_parent == 0 || dest_parent->type != FILE_TYPE_DIRECTORY) {
        std::cout << "mv: 目标目录不存在" << std::endl;
        return;
    }

    // 构建目标目录路径
    get_current_path(temp_path, dest_parent);

    // 检查是否在同一目录且只是重命名
    int same_dir = (dest_parent == src_file->parent);

    if (same_dir) {
        // 同一目录,直接重命名
        if (str_compare(src_file->filename, dest_filename) == 0) {
            return;  // 名字相同,无需操作
        }

        FileSystemStatus status = fs_rename(src_full, dest_filename, g_current_uid, g_current_gid);
        if (status != FS_SUCCESS) {
            if (status == FS_ERR_PERMISSION) {
                std::cout << "mv: 权限不足" << std::endl;
            } else if (status == FS_ERR_EXISTS) {
                std::cout << "mv: 目标文件已存在" << std::endl;
            } else {
                std::cout << "mv: 重命名失败" << std::endl;
            }
        }
    } else {
        // 不同目录,先移动再重命名(如果需要)
        FileSystemStatus status = fs_move(src_full, temp_path, g_current_uid, g_current_gid);
        if (status != FS_SUCCESS) {
            if (status == FS_ERR_PERMISSION) {
                std::cout << "mv: 权限不足" << std::endl;
            } else if (status == FS_ERR_EXISTS) {
                std::cout << "mv: 目标文件已存在" << std::endl;
            } else {
                std::cout << "mv: 移动失败" << std::endl;
            }
            return;
        }

        // 如果需要重命名
        if (str_compare(src_file->filename, dest_filename) != 0) {
            // 构建移动后的完整路径
            get_current_path(temp_path, dest_parent);
            if (str_compare(temp_path, "/") != 0) {
                str_concat(temp_path, "/");
            }
            str_concat(temp_path, src_file->filename);

            status = fs_rename(temp_path, dest_filename, g_current_uid, g_current_gid);
            if (status != FS_SUCCESS) {
                if (status == FS_ERR_PERMISSION) {
                    std::cout << "mv: 移动成功但重命名失败(权限不足)" << std::endl;
                } else if (status == FS_ERR_EXISTS) {
                    std::cout << "mv: 移动成功但重命名失败(目标已存在)" << std::endl;
                } else {
                    std::cout << "mv: 移动成功但重命名失败" << std::endl;
                }
            }
        }
    }
}

// 命令: cp - 复制文件
void cmd_cp(const char* src_path, const char* dest_path) {
    char src_full[MAX_PATH_LEN];
    char dest_dirname[MAX_PATH_LEN];
    char dest_filename[MAX_FILENAME_LEN];
    char temp_path[MAX_PATH_LEN];
    FileNode* src_file;
    FileNode* dest_node;
    FileNode* dest_parent;

    if (src_path == 0 || dest_path == 0 ||
        str_length(src_path) == 0 || str_length(dest_path) == 0) {
        std::cout << "cp: 缺少参数" << std::endl;
        return;
    }

    // 解析源路径
    if (src_path[0] == '/') {
        str_copy(src_full, src_path);
    } else {
        src_file = fs_find_relative(g_current_dir, src_path);
        if (src_file == 0) {
            std::cout << "cp: 源文件不存在" << std::endl;
            return;
        }
        get_current_path(src_full, src_file);
    }

    // 检查源文件
    src_file = fs_find(src_full);
    if (src_file == 0) {
        std::cout << "cp: 源文件不存在" << std::endl;
        return;
    }

    // 解析目标路径
    // 首先尝试查找目标是否存在
    if (dest_path[0] == '/') {
        dest_node = fs_find(dest_path);
    } else {
        dest_node = fs_find_relative(g_current_dir, dest_path);
    }

    if (dest_node != 0 && dest_node->type == FILE_TYPE_DIRECTORY) {
        // 目标是目录,复制到该目录中,保持原文件名
        str_copy(dest_dirname, dest_path);
        str_copy(dest_filename, src_file->filename);
    } else {
        // 目标不是目录或不存在,解析为目录+文件名
        if (fs_parse_path(dest_path, dest_dirname, dest_filename) != 0) {
            std::cout << "cp: 目标路径无效" << std::endl;
            return;
        }

        // 如果文件名为空,使用源文件名
        if (str_length(dest_filename) == 0) {
            str_copy(dest_filename, src_file->filename);
        }
    }

    // 解析目标目录
    if (dest_dirname[0] == '\0') {
        dest_parent = g_current_dir;
    } else if (dest_dirname[0] == '/') {
        dest_parent = fs_find(dest_dirname);
    } else {
        dest_parent = fs_find_relative(g_current_dir, dest_dirname);
    }

    if (dest_parent == 0 || dest_parent->type != FILE_TYPE_DIRECTORY) {
        std::cout << "cp: 目标目录不存在" << std::endl;
        return;
    }

    // 构建目标目录路径
    get_current_path(temp_path, dest_parent);

    // 先复制到目标目录
    FileSystemStatus status = fs_copy(src_full, temp_path, g_current_uid, g_current_gid);
    if (status != FS_SUCCESS) {
        if (status == FS_ERR_PERMISSION) {
            std::cout << "cp: 权限不足" << std::endl;
        } else if (status == FS_ERR_EXISTS) {
            std::cout << "cp: 目标文件已存在" << std::endl;
        } else {
            std::cout << "cp: 复制失败" << std::endl;
        }
        return;
    }

    // 如果需要重命名
    if (str_compare(src_file->filename, dest_filename) != 0) {
        // 构建复制后的完整路径
        get_current_path(temp_path, dest_parent);
        if (str_compare(temp_path, "/") != 0) {
            str_concat(temp_path, "/");
        }
        str_concat(temp_path, src_file->filename);

        status = fs_rename(temp_path, dest_filename, g_current_uid, g_current_gid);
        if (status != FS_SUCCESS) {
            if (status == FS_ERR_PERMISSION) {
                std::cout << "cp: 复制成功但重命名失败(权限不足)" << std::endl;
            } else if (status == FS_ERR_EXISTS) {
                std::cout << "cp: 复制成功但重命名失败(目标已存在)" << std::endl;
            } else {
                std::cout << "cp: 复制成功但重命名失败" << std::endl;
            }
        }
    }
}

// 命令: chmod - 修改权限
void cmd_chmod(const char* path, const char* perms_str) {
    int owner_perms = 0, group_perms = 0, other_perms = 0;
    Permissions perms;

    if (path == 0 || perms_str == 0 ||
        str_length(path) == 0 || str_length(perms_str) == 0) {
        std::cout << "chmod: 缺少参数" << std::endl;
        return;
    }

    // 解析权限字符串 (例如: 755 或 rwxr-xr-x)
    if (str_length(perms_str) == 3) {
        // 八进制格式 (例如: 400, 755)
        int p = 0;
        for (int i = 0; i < 3; i++) {
            char c = perms_str[i];
            if (c >= '0' && c <= '7') {
                p = p * 8 + (c - '0');  // 八进制转十进制
            } else {
                std::cout << "chmod: 无效权限格式" << std::endl;
                return;
            }
        }
        // 将八进制数字转换为权限位组合
        int owner_octal = (p >> 6) & 7;
        int group_octal = (p >> 3) & 7;
        int other_octal = p & 7;

        // 转换: 八进制数字 -> 权限位
        // 4(100) -> r--, 2(010) -> -w-, 1(001) -> --x
        owner_perms = 0;
        if (owner_octal & 4) owner_perms |= PERM_READ;
        if (owner_octal & 2) owner_perms |= PERM_WRITE;
        if (owner_octal & 1) owner_perms |= PERM_EXECUTE;

        group_perms = 0;
        if (group_octal & 4) group_perms |= PERM_READ;
        if (group_octal & 2) group_perms |= PERM_WRITE;
        if (group_octal & 1) group_perms |= PERM_EXECUTE;

        other_perms = 0;
        if (other_octal & 4) other_perms |= PERM_READ;
        if (other_octal & 2) other_perms |= PERM_WRITE;
        if (other_octal & 1) other_perms |= PERM_EXECUTE;
    } else if (str_length(perms_str) == 9) {
        // 符号格式 rwxr-xr-x
        if (perms_str[0] == 'r') owner_perms |= PERM_READ;
        if (perms_str[1] == 'w') owner_perms |= PERM_WRITE;
        if (perms_str[2] == 'x') owner_perms |= PERM_EXECUTE;
        if (perms_str[3] == 'r') group_perms |= PERM_READ;
        if (perms_str[4] == 'w') group_perms |= PERM_WRITE;
        if (perms_str[5] == 'x') group_perms |= PERM_EXECUTE;
        if (perms_str[6] == 'r') other_perms |= PERM_READ;
        if (perms_str[7] == 'w') other_perms |= PERM_WRITE;
        if (perms_str[8] == 'x') other_perms |= PERM_EXECUTE;
    } else {
        std::cout << "chmod: 无效权限格式" << std::endl;
        return;
    }

    perms.owner_perms = owner_perms;
    perms.group_perms = group_perms;
    perms.other_perms = other_perms;

    FileSystemStatus status = fs_chmod(path, perms);
    if (status != FS_SUCCESS) {
        if (status == FS_ERR_PERMISSION) {
            std::cout << "chmod: 权限不足" << std::endl;
        } else if (status == FS_ERR_NOT_FOUND) {
            std::cout << "chmod: 文件不存在" << std::endl;
        } else {
            std::cout << "chmod: 修改失败" << std::endl;
        }
    }
}

// 命令: chown - 修改所有者
void cmd_chown(const char* path, const char* uid_str, const char* gid_str) {
    int uid, gid;

    if (path == 0 || uid_str == 0 ||
        str_length(path) == 0 || str_length(uid_str) == 0) {
        std::cout << "chown: 缺少参数" << std::endl;
        std::cout << "用法: chown <path> <uid> [gid]" << std::endl;
        return;
    }

    uid = atoi(uid_str);
    gid = (gid_str != 0 && str_length(gid_str) > 0) ? atoi(gid_str) : -1;  // -1 表示不修改GID

    FileSystemStatus status = fs_chown(path, uid, gid);
    if (status != FS_SUCCESS) {
        if (status == FS_ERR_PERMISSION) {
            std::cout << "chown: 权限不足（只有root可以修改所有者）" << std::endl;
        } else if (status == FS_ERR_NOT_FOUND) {
            std::cout << "chown: 文件不存在" << std::endl;
        } else {
            std::cout << "chown: 修改失败" << std::endl;
        }
    } else {
        if (gid != -1) {
            std::cout << "修改所有者成功: UID=" << uid << ", GID=" << gid << std::endl;
        } else {
            std::cout << "修改所有者成功: UID=" << uid << std::endl;
        }
    }
}

// 命令: cd - 切换目录
void cmd_cd(const char* path) {
    FileNode* new_dir;

    if (path == 0 || str_length(path) == 0) {
        // cd 不带参数，回到根目录
        new_dir = fs_get_root();
    } else if (str_compare(path, "~") == 0) {
        // cd ~ 回到根目录
        new_dir = fs_get_root();
    } else if (str_compare(path, ".") == 0 || str_compare(path, "./") == 0) {
        // cd . 或 cd ./ 表示当前目录
        new_dir = g_current_dir;
    } else if (path[0] == '/') {
        // 绝对路径
        new_dir = fs_find(path);
    } else {
        // 相对路径
        new_dir = fs_find_relative(g_current_dir, path);
    }

    if (new_dir == 0) {
        std::cout << "cd: 目录不存在" << std::endl;
        return;
    }

    if (new_dir->type != FILE_TYPE_DIRECTORY) {
        std::cout << "cd: 不是目录" << std::endl;
        return;
    }

    // 检查执行权限
    if (fs_check_permission(new_dir, g_current_uid, g_current_gid, OP_EXECUTE) != PERM_GRANTED) {
        std::cout << "cd: 权限不足" << std::endl;
        return;
    }

    g_current_dir = new_dir;
}

// 命令: pwd - 显示当前目录
void cmd_pwd() {
    char path[MAX_PATH_LEN];
    get_current_path(path, g_current_dir);
    std::cout << path << std::endl;
}

// 命令: su - 切换用户
void cmd_su(const char* uid_str) {
    if (uid_str == 0 || str_length(uid_str) == 0) {
        std::cout << "su: 缺少参数" << std::endl;
        return;
    }

    int new_uid = atoi(uid_str);
    User* user = um_find_user_by_uid(new_uid);
    if (user == 0) {
        std::cout << "su: 用户不存在" << std::endl;
        return;
    }

    g_current_uid = new_uid;
    g_current_gid = user->gid;

    // 同步到内核层
    fs_set_current_user(g_current_uid, g_current_gid);
}

// 命令: whoami - 显示当前用户
void cmd_whoami() {
    std::cout << get_username(g_current_uid) << " (UID=" << g_current_uid << ")" << std::endl;
}

// 命令: useradd - 创建用户
void cmd_useradd(const char* username) {
    if (username == 0 || str_length(username) == 0) {
        std::cout << "useradd: 缺少用户名" << std::endl;
        return;
    }

    // 只有root可以创建用户
    if (g_current_uid != 0) {
        std::cout << "useradd: 只有root可以创建用户" << std::endl;
        return;
    }

    // 自动分配UID (从当前用户数+1开始)
    int new_uid = um_get_user_count();
    int new_gid = new_uid;  // 简化: UID和GID相同

    UserStatus status = um_create_user(username, new_uid, new_gid);
    if (status != USER_SUCCESS) {
        std::cout << "useradd: 创建失败" << std::endl;
    } else {
        std::cout << "用户创建成功: " << username << " (UID=" << new_uid << ", GID=" << new_gid << ")" << std::endl;
    }
}

// 命令: userdel - 删除用户
void cmd_userdel(const char* uid_str) {
    if (uid_str == 0 || str_length(uid_str) == 0) {
        std::cout << "userdel: 缺少用户ID" << std::endl;
        return;
    }

    // 只有root可以删除用户
    if (g_current_uid != 0) {
        std::cout << "userdel: 只有root可以删除用户" << std::endl;
        return;
    }

    int uid = atoi(uid_str);
    UserStatus status = um_delete_user(uid);
    if (status != USER_SUCCESS) {
        std::cout << "userdel: 删除失败" << std::endl;
    } else {
        std::cout << "用户删除成功: UID=" << uid << std::endl;
    }
}

// 命令: groupadd - 创建组
void cmd_groupadd(const char* groupname) {
    if (groupname == 0 || str_length(groupname) == 0) {
        std::cout << "groupadd: 缺少组名" << std::endl;
        return;
    }

    // 只有root可以创建组
    if (g_current_uid != 0) {
        std::cout << "groupadd: 只有root可以创建组" << std::endl;
        return;
    }

    // 自动分配GID
    int new_gid = um_get_group_count();

    GroupStatus status = um_create_group(groupname, new_gid);
    if (status != GROUP_SUCCESS) {
        std::cout << "groupadd: 创建失败" << std::endl;
    } else {
        std::cout << "组创建成功: " << groupname << " (GID=" << new_gid << ")" << std::endl;
    }
}

// 命令: groupmod - 添加用户到组
void cmd_groupmod(const char* gid_str, const char* uid_str) {
    if (gid_str == 0 || str_length(gid_str) == 0) {
        std::cout << "groupmod: 缺少GID参数" << std::endl;
        std::cout << "用法: groupmod <gid> <uid>" << std::endl;
        return;
    }

    if (uid_str == 0 || str_length(uid_str) == 0) {
        std::cout << "groupmod: 缺少UID参数" << std::endl;
        std::cout << "用法: groupmod <gid> <uid>" << std::endl;
        return;
    }

    // 只有root可以修改组成员
    if (g_current_uid != 0) {
        std::cout << "groupmod: 只有root可以修改组成员" << std::endl;
        return;
    }

    // 转换GID和UID
    int gid = atoi(gid_str);
    int uid = atoi(uid_str);

    // 验证用户存在
    if (uid < 0 || uid >= um_get_user_count()) {
        std::cout << "groupmod: 用户不存在 (UID=" << uid << ")" << std::endl;
        return;
    }

    // 验证组存在
    if (gid < 0 || gid >= um_get_group_count()) {
        std::cout << "groupmod: 组不存在 (GID=" << gid << ")" << std::endl;
        return;
    }

    // 添加成员到组
    GroupStatus status = um_add_group_member(gid, uid);
    if (status == GROUP_SUCCESS) {
        std::cout << "成功将 UID=" << uid << " 添加到 GID=" << gid << std::endl;
    } else if (status == GROUP_ERR_EXISTS) {
        std::cout << "用户已在该组中" << std::endl;
    } else {
        std::cout << "groupmod: 添加成员失败" << std::endl;
    }
}

// 命令: help - 显示帮助
void cmd_help() {
    std::cout << "\n可用命令:" << std::endl;
    std::cout << "  useradd <username>     - 创建新用户" << std::endl;
    std::cout << "  userdel <uid>          - 删除用户" << std::endl;
    std::cout << "  groupadd <groupname>   - 创建新组" << std::endl;
    std::cout << "  groupmod <gid> <uid>   - 添加用户到组" << std::endl;
    std::cout << "  ls [path]              - 列出目录内容" << std::endl;
    std::cout << "  mkdir <path>           - 创建目录" << std::endl;
    std::cout << "  touch <path>           - 创建文件" << std::endl;
    std::cout << "  rm <path>              - 删除文件或空目录" << std::endl;
    std::cout << "  rm -r <path>           - 递归删除" << std::endl;
    std::cout << "  mv <src> <dest>        - 移动/重命名文件" << std::endl;
    std::cout << "  cp <src> <dest>        - 复制文件" << std::endl;
    std::cout << "  chmod <path> <perms>   - 修改权限 (755或rwxr-xr-x)" << std::endl;
    std::cout << "  chown <path> <uid>     - 修改所有者" << std::endl;
    std::cout << "  cd <path>              - 切换目录" << std::endl;
    std::cout << "  pwd                    - 显示当前目录" << std::endl;
    std::cout << "  su <uid>               - 切换用户" << std::endl;
    std::cout << "  whoami                 - 显示当前用户" << std::endl;
    std::cout << "  help                   - 显示此帮助" << std::endl;
    std::cout << "  exit                   - 退出Shell" << std::endl;
    std::cout << "\n权限格式示例:" << std::endl;
    std::cout << "  755 (八进制) 或 rwxr-xr-x (符号)" << std::endl;
}

// 执行命令
int execute_command(const char* input) {
    char cmd[32];
    char args[3][MAX_PATH_LEN];
    int argc = 0;
    int i = 0;
    int j = 0;

    // 跳过前导空格
    while (input[i] == ' ' && input[i] != '\0') i++;

    if (input[i] == '\0' || input[i] == '\n') {
        return 0;  // 空命令
    }

    // 提取命令
    j = 0;
    while (input[i] != ' ' && input[i] != '\0' && input[i] != '\n' && j < 31) {
        cmd[j++] = input[i++];
    }
    cmd[j] = '\0';

    // 提取参数
    for (int arg_idx = 0; arg_idx < 3; arg_idx++) {
        // 跳过空格
        while (input[i] == ' ' && input[i] != '\0') i++;

        if (input[i] == '\0' || input[i] == '\n') {
            args[arg_idx][0] = '\0';
            break;
        }

        // 提取参数
        j = 0;
        while (input[i] != ' ' && input[i] != '\0' && input[i] != '\n' && j < MAX_PATH_LEN - 1) {
            args[arg_idx][j++] = input[i++];
        }
        args[arg_idx][j] = '\0';
        argc++;
    }

    // 执行命令
    if (str_compare(cmd, "ls") == 0) {
        cmd_ls(args[0]);
    } else if (str_compare(cmd, "mkdir") == 0) {
        cmd_mkdir(args[0]);
    } else if (str_compare(cmd, "touch") == 0) {
        cmd_touch(args[0]);
    } else if (str_compare(cmd, "rm") == 0) {
        if (argc > 1 && str_compare(args[0], "-r") == 0) {
            cmd_rm(args[1], 1);
        } else {
            cmd_rm(args[0], 0);
        }
    } else if (str_compare(cmd, "mv") == 0) {
        cmd_mv(args[0], args[1]);
    } else if (str_compare(cmd, "cp") == 0) {
        cmd_cp(args[0], args[1]);
    } else if (str_compare(cmd, "chmod") == 0) {
        cmd_chmod(args[0], args[1]);
    } else if (str_compare(cmd, "chown") == 0) {
        cmd_chown(args[0], args[1], args[2]);
    } else if (str_compare(cmd, "cd") == 0) {
        cmd_cd(args[0]);
    } else if (str_compare(cmd, "pwd") == 0) {
        cmd_pwd();
    } else if (str_compare(cmd, "su") == 0) {
        cmd_su(args[0]);
    } else if (str_compare(cmd, "whoami") == 0) {
        cmd_whoami();
    } else if (str_compare(cmd, "useradd") == 0) {
        cmd_useradd(args[0]);
    } else if (str_compare(cmd, "userdel") == 0) {
        cmd_userdel(args[0]);
    } else if (str_compare(cmd, "groupadd") == 0) {
        cmd_groupadd(args[0]);
    } else if (str_compare(cmd, "groupmod") == 0) {
        cmd_groupmod(args[0], args[1]);
    } else if (str_compare(cmd, "help") == 0) {
        cmd_help();
    } else if (str_compare(cmd, "exit") == 0) {
        return -1;  // 退出Shell
    } else {
        std::cout << "未知命令: " << cmd << " (输入 'help' 查看帮助)" << std::endl;
    }

    return 0;
}

// Shell主循环
void shell_loop() {
    char input[256];

    std::cout << "\n========================================" << std::endl;
    std::cout << "UNIX 文件权限模拟器 - 交互式Shell" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "输入 'help' 查看可用命令" << std::endl;
    std::cout << std::endl;

    // 初始化当前目录为根目录
    g_current_dir = fs_get_root();

    while (1) {
        print_prompt();

        // 读取输入
        if (std::cin.getline(input, sizeof(input))) {
            if (execute_command(input) == -1) {
                break;
            }
        }
    }

    std::cout << "\n再见!" << std::endl;
}

int main()
{
    SetConsoleOutputCP(65001);
    std::cout << "========================================" << std::endl;
    std::cout << "UNIX 文件权限模拟器" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "请选择模式:" << std::endl;
    std::cout << "  1. 运行所有测试" << std::endl;
    std::cout << "  2. 交互式Shell" << std::endl;
    std::cout << "请输入选择 (1 或 2): ";

    char choice[10];
    std::cin.getline(choice, sizeof(choice));

    // 初始化用户管理系统
    um_init();

    if (str_compare(choice, "2") == 0) {
        // Shell 模式
        // 初始化文件系统
        fs_init();

        // 创建root用户
        um_create_user("root", 0, 0);

        // 设置默认用户为root
        fs_set_current_user(0, 0);
        g_current_uid = 0;
        g_current_gid = 0;

        shell_loop();
    } else {
        // 测试模式
        std::cout << "\n========================================" << std::endl;
        std::cout << "UNIX 文件权限模拟器 - 完整测试" << std::endl;
        std::cout << "========================================" << std::endl;

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

        // 第三阶段续: 路径解析功能测试
        std::cout << "\n############ 第三阶段续: 路径解析功能测试 ############" << std::endl;
        test_normalize_path();
        test_find_relative();

        // 第三阶段任务六: 批量测试脚本
        std::cout << "\n############ 第三阶段任务六: 批量测试脚本 ############" << std::endl;
        test_comprehensive_scenario();
        test_multi_user_collaboration();
        test_edge_cases();

        std::cout << "\n========================================" << std::endl;
        std::cout << "所有测试完成!" << std::endl;
        std::cout << "========================================" << std::endl;
    }

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
