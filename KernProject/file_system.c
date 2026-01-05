#include "file_system.h"
#include "user_manager.h"

// ==================== 全局变量 ====================

static FileNode* g_root = 0;                     // 根目录指针
static FileNode g_file_nodes[MAX_FILES];         // 文件节点池
static int g_file_count = 0;                     // 当前文件节点数量
static int g_next_inode = 1;                     // 下一个可用的 inode 号
static int g_current_uid = 0;                    // 当前用户ID
static int g_current_gid = 0;                    // 当前用户组ID

// ==================== 内部辅助函数 ====================

/**
 * 字符串长度计算（不使用标准库，带边界检查）
 * @param str 字符串
 * @return 字符串长度
 */
static int str_length(const char* str)
{
    int len = 0;
    if (str == 0) return 0;

    int max_len = MAX_PATH_LEN * 2;  // 防止无限循环
    while (str[len] != '\0' && len < max_len) {
        len++;
    }
    return len;
}

/**
 * 字符串比较（不使用标准库，带边界检查）
 * @param s1 字符串1
 * @param s2 字符串2
 * @return 相等返回0，不相等返回非0
 */
static int str_compare(const char* s1, const char* s2)
{
    if (s1 == 0 || s2 == 0) return -1;

    int i = 0;
    int max_cmp = MAX_PATH_LEN;  // 限制比较长度，防止溢出
    while (s1[i] != '\0' && s2[i] != '\0' && i < max_cmp) {
        if (s1[i] != s2[i]) {
            return s1[i] - s2[i];
        }
        i++;
    }
    return s1[i] - s2[i];
}

/**
 * 字符串复制（不使用标准库，带边界检查）
 * @param dest 目标缓冲区
 * @param src 源字符串
 * @param max_len 缓冲区最大长度
 */
static void str_copy(char* dest, const char* src, int max_len)
{
    if (dest == 0 || src == 0) return;

    int i = 0;
    while (src[i] != '\0' && i < max_len - 1) {  // 保留空间给 '\0'
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';  // 确保以 null 结尾
}

/**
 * 字符串拼接（不使用标准库，带边界检查）
 * @param dest 目标缓冲区
 * @param src 要追加的源字符串
 * @param max_len 缓冲区最大长度
 */
static void str_concat(char* dest, const char* src, int max_len)
{
    if (dest == 0 || src == 0) return;

    int i = 0;
    int j = 0;

    // 找到 dest 的末尾
    while (dest[i] != '\0' && i < max_len) {
        i++;
    }

    // 追加 src
    while (src[j] != '\0' && i < max_len - 1) {  // 保留空间给 '\0'
        dest[i++] = src[j++];
    }
    dest[i] = '\0';  // 确保以 null 结尾
}

/**
 * 分配新的文件节点
 * @return 新文件节点指针，失败返回NULL
 */
static FileNode* allocate_file_node(void)
{
    int i;

    if (g_file_count >= MAX_FILES) {
        return 0;
    }

    // 从节点池中查找空闲节点
    for (i = 0; i < MAX_FILES; i++) {
        if (g_file_nodes[i].inode == 0) {
            g_file_nodes[i].inode = g_next_inode++;
            g_file_count++;
            return &g_file_nodes[i];
        }
    }

    return 0;
}

/**
 * 释放文件节点
 * @param node 要释放的节点
 */
static void free_file_node(FileNode* node)
{
    if (node == 0) return;

    // 清零节点数据
    node->filename[0] = '\0';
    node->type = FILE_TYPE_REGULAR;
    node->uid = 0;
    node->gid = 0;
    node->perms.owner_perms = 0;
    node->perms.group_perms = 0;
    node->perms.other_perms = 0;
    node->parent = 0;
    node->children = 0;
    node->next = 0;
    node->inode = 0;

    g_file_count--;
}

/**
 * 计算目录中的文件数量
 * @param dir 目录节点
 * @return 文件数量
 */
static int count_directory_children(FileNode* dir)
{
    int count = 0;
    FileNode* child;

    if (dir == 0 || dir->type != FILE_TYPE_DIRECTORY) {
        return 0;
    }

    child = dir->children;
    while (child != 0) {
        count++;
        child = child->next;
    }

    return count;
}

// ==================== 文件系统管理函数 ====================

/**
 * 初始化文件系统
 */
void fs_init(void)
{
    int i;

    // 初始化文件节点池
    for (i = 0; i < MAX_FILES; i++) {
        g_file_nodes[i].inode = 0;
        g_file_nodes[i].filename[0] = '\0';
        g_file_nodes[i].type = FILE_TYPE_REGULAR;
        g_file_nodes[i].uid = 0;
        g_file_nodes[i].gid = 0;
        g_file_nodes[i].perms.owner_perms = 0;
        g_file_nodes[i].perms.group_perms = 0;
        g_file_nodes[i].perms.other_perms = 0;
        g_file_nodes[i].parent = 0;
        g_file_nodes[i].children = 0;
        g_file_nodes[i].next = 0;
    }

    g_file_count = 0;
    g_next_inode = 1;

    // 创建根目录
    FileNode* root = allocate_file_node();
    if (root == 0) {
        return;  // 分配失败
    }

    // 初始化根目录属性
    str_copy(root->filename, "", MAX_FILENAME_LEN);
    root->type = FILE_TYPE_DIRECTORY;
    root->uid = 0;                    // root 用户
    root->gid = 0;                    // root 组
    root->perms.owner_perms = PERM_READ | PERM_WRITE | PERM_EXECUTE;
    root->perms.group_perms = PERM_READ | PERM_EXECUTE;
    root->perms.other_perms = PERM_READ | PERM_EXECUTE;
    root->parent = 0;
    root->children = 0;
    root->next = 0;

    g_root = root;
}

/**
 * 获取根目录节点
 */
FileNode* fs_get_root(void)
{
    return g_root;
}

/**
 * 获取文件数量统计
 */
int fs_get_file_count(void)
{
    return g_file_count;
}

/**
 * 设置当前用户
 */
void fs_set_current_user(int uid, int gid)
{
    g_current_uid = uid;
    g_current_gid = gid;
}

/**
 * 获取当前用户ID
 */
int fs_get_current_uid(void)
{
    return g_current_uid;
}

/**
 * 获取当前用户组ID
 */
int fs_get_current_gid(void)
{
    return g_current_gid;
}

// ==================== 辅助函数实现 ====================

/**
 * 在目录中查找子文件
 */
FileNode* fs_find_in_directory(FileNode* dir, const char* filename)
{
    FileNode* child;

    // 参数验证
    if (dir == 0 || filename == 0) {
        return 0;
    }

    // 必须是目录
    if (dir->type != FILE_TYPE_DIRECTORY) {
        return 0;
    }

    // 遍历子节点链表
    child = dir->children;
    while (child != 0) {
        if (str_compare(child->filename, filename) == 0) {
            return child;  // 找到匹配的文件
        }
        child = child->next;
    }

    return 0;  // 未找到
}

/**
 * 路径解析辅助函数(带边界检查)
 */
int fs_parse_path(const char* path, char* dirname, char* filename)
{
    int len;
    int last_slash;
    int i;

    // 参数验证
    if (path == 0 || dirname == 0 || filename == 0) {
        return -1;
    }

    len = str_length(path);
    if (len == 0 || len >= MAX_PATH_LEN) {
        return -1;  // 路径过长或为空
    }

    // 路径必须以 / 开头
    if (path[0] != '/') {
        return -1;
    }

    // 处理根目录
    if (len == 1) {
        dirname[0] = '\0';
        filename[0] = '\0';
        return 0;
    }

    // 查找最后一个 /
    last_slash = len - 1;
    while (last_slash > 0 && path[last_slash] != '/') {
        last_slash--;
    }

    if (last_slash == 0) {
        // 文件在根目录下
        if (len - 1 >= MAX_FILENAME_LEN) {
            return -1;  // 文件名过长
        }
        dirname[0] = '/';
        dirname[1] = '\0';

        // 复制文件名
        for (i = 1; i < len && i - 1 < MAX_FILENAME_LEN - 1; i++) {
            filename[i - 1] = path[i];
        }
        filename[i - 1] = '\0';
    } else {
        // 检查目录部分长度
        if (last_slash >= MAX_PATH_LEN) {
            return -1;  // 目录路径过长
        }

        // 复制目录部分
        for (i = 0; i < last_slash && i < MAX_PATH_LEN - 1; i++) {
            dirname[i] = path[i];
        }
        dirname[i] = '\0';

        // 检查文件名长度
        int filename_len = len - last_slash - 1;
        if (filename_len >= MAX_FILENAME_LEN) {
            return -1;  // 文件名过长
        }

        // 复制文件名部分
        for (i = last_slash + 1; i < len && i - last_slash - 1 < MAX_FILENAME_LEN - 1; i++) {
            filename[i - last_slash - 1] = path[i];
        }
        filename[i - last_slash - 1] = '\0';
    }

    return 0;
}

/**
 * 规范化路径字符串(优化版本: 减少中间字符串操作,带边界检查)
 * 处理 . 和 ..，合并连续的 /，移除末尾的 / (根目录除外)
 */
int fs_normalize_path(const char* input, char* output)
{
    // 使用指针和长度而不是复制字符串
    const char* component_ptrs[MAX_PATH_LEN / 2];  // 组件指针数组
    int component_lens[MAX_PATH_LEN / 2];           // 组件长度数组
    int component_count = 0;
    int i = 0;
    int is_absolute = 0;
    int input_len = 0;

    // 参数验证
    if (input == 0 || output == 0) {
        return -1;
    }

    // 检查输入长度
    input_len = str_length(input);
    if (input_len >= MAX_PATH_LEN) {
        return -1;  // 路径过长
    }

    // 判断是否为绝对路径
    if (input[0] == '/') {
        is_absolute = 1;
        i = 1;  // 跳过开头的 /
    }

    // 解析路径组件
    while (input[i] != '\0' && i < input_len) {
        // 跳过连续的 /
        while (input[i] == '/' && i < input_len) {
            i++;
        }

        if (input[i] == '\0' || i >= input_len) {
            break;
        }

        // 记录组件起始位置
        const char* comp_start = &input[i];
        int comp_len = 0;

        // 计算组件长度
        while (input[i] != '\0' && input[i] != '/' && comp_len < MAX_FILENAME_LEN - 1 && i < input_len) {
            i++;
            comp_len++;
        }

        // 处理特殊组件
        if (comp_len == 1 && comp_start[0] == '.') {
            // 当前目录，忽略
            continue;
        } else if (comp_len == 2 && comp_start[0] == '.' && comp_start[1] == '.') {
            // 父目录，弹出上一个组件(如果有的话)
            if (component_count > 0) {
                component_count--;
            }
            // 如果是绝对路径，不能超出根目录
            // 如果是相对路径，可以添加 .. 组件
            if (!is_absolute && component_count == 0) {
                // 检查数组边界
                if (component_count < MAX_PATH_LEN / 2) {
                    component_ptrs[component_count] = comp_start;
                    component_lens[component_count] = comp_len;
                    component_count++;
                } else {
                    return -1;  // 组件过多
                }
            }
        } else {
            // 普通组件，记录指针和长度
            // 检查数组边界
            if (component_count < MAX_PATH_LEN / 2 - 1) {
                component_ptrs[component_count] = comp_start;
                component_lens[component_count] = comp_len;
                component_count++;
            } else {
                return -1;  // 组件过多
            }
        }
    }

    // 构建规范化路径(直接从源字符串复制,避免中间缓冲)
    i = 0;

    // 检查输出缓冲区边界
    if (is_absolute && i < MAX_PATH_LEN - 1) {
        output[i++] = '/';
    }

    for (int j = 0; j < component_count; j++) {
        int len = component_lens[j];
        const char* ptr = component_ptrs[j];

        // 检查缓冲区空间
        if (i + len + (j > 0 ? 1 : 0) >= MAX_PATH_LEN) {
            return -1;  // 路径过长
        }

        // 添加 / 分隔符(除第一个组件外)
        if (j > 0 && i < MAX_PATH_LEN - 1) {
            output[i++] = '/';
        }

        // 直接复制组件(无需中间字符串),带边界检查
        for (int k = 0; k < len && i < MAX_PATH_LEN - 1; k++) {
            output[i++] = ptr[k];
        }
    }

    // 处理空路径
    if (i == 0) {
        if (is_absolute && i < MAX_PATH_LEN - 1) {
            output[i++] = '/';
        } else if (i < MAX_PATH_LEN - 1) {
            // 相对路径为空，使用 .
            output[i++] = '.';
        }
    }

    // 确保不超过缓冲区边界
    if (i >= MAX_PATH_LEN) {
        i = MAX_PATH_LEN - 1;
    }
    output[i] = '\0';

    return 0;
}

/**
 * 从指定目录开始查找相对路径
 * 支持 . (当前目录) 和 .. (父目录)
 */
FileNode* fs_find_relative(FileNode* current_dir, const char* path)
{
    char normalized[MAX_PATH_LEN];
    FileNode* current;
    int i;
    int start;
    int end;
    char component[MAX_FILENAME_LEN];

    // 参数验证
    if (path == 0) {
        return 0;
    }

    // 规范化路径
    if (fs_normalize_path(path, normalized) != 0) {
        return 0;
    }

    // 处理绝对路径
    if (normalized[0] == '/') {
        return fs_find(normalized);
    }

    // 处理相对路径
    // 从当前目录开始
    current = current_dir;
    if (current == 0) {
        current = g_root;  // 默认从根目录开始
    }

    // 必须是目录
    if (current->type != FILE_TYPE_DIRECTORY) {
        return 0;
    }

    // 处理空路径（规范化后的 "." 或 "./" 表示当前目录）
    if (normalized[0] == '\0') {
        return current;
    }

    start = 0;

    while (normalized[start] != '\0') {
        // 查找下一个 /
        end = start;
        while (normalized[end] != '\0' && normalized[end] != '/') {
            end++;
        }

        // 提取路径组件
        if (end - start >= MAX_FILENAME_LEN) {
            return 0;  // 文件名过长
        }

        for (i = 0; i < end - start; i++) {
            component[i] = normalized[start + i];
        }
        component[end - start] = '\0';

        // 处理 ..
        if (str_compare(component, "..") == 0) {
            // 移动到父目录
            if (current->parent != 0) {
                current = current->parent;
            }
            // 如果已经在根目录，保持不动
        } else {
            // 在当前目录中查找
            current = fs_find_in_directory(current, component);
            if (current == 0) {
                return 0;  // 路径不存在
            }
        }

        // 移动到下一个组件
        if (normalized[end] == '/') {
            start = end + 1;
        } else {
            break;
        }
    }

    return current;
}

// ==================== 权限检查函数 ====================

/**
 * 检查用户对文件是否有执行某操作的权限
 */
PermissionResult fs_check_permission(FileNode* file, int uid, int gid, OperationType op)
{
    unsigned char required_perms;
    unsigned char user_perms;
    int is_member;

    // 参数验证
    if (file == 0) {
        return PERM_ERROR;
    }

    // root 用户拥有所有权限
    if (uid == 0) {
        return PERM_GRANTED;
    }

    // 确定需要的权限位
    switch (op) {
        case OP_READ:
            required_perms = PERM_READ;
            break;
        case OP_WRITE:
            required_perms = PERM_WRITE;
            break;
        case OP_EXECUTE:
            required_perms = PERM_EXECUTE;
            break;
        case OP_DELETE:
            required_perms = PERM_WRITE;  // 删除需要写权限
            break;
        default:
            return PERM_ERROR;
    }

    // 确定用户应该检查哪一组权限
    if (uid == file->uid) {
        // 文件所有者
        user_perms = file->perms.owner_perms;
    } else {
        // 检查是否属于文件所属组
        is_member = um_is_group_member(file->gid, uid);
        if (is_member) {
            // 组成员
            user_perms = file->perms.group_perms;
        } else {
            // 其他用户
            user_perms = file->perms.other_perms;
        }
    }

    // 检查是否有所需权限
    if ((user_perms & required_perms) != 0) {
        return PERM_GRANTED;
    } else {
        return PERM_DENIED;
    }
}

/**
 * 根据路径查找文件节点
 */
FileNode* fs_find(const char* path)
{
    FileNode* current;
    int i;
    int start;
    int end;
    char component[MAX_FILENAME_LEN];

    // 参数验证
    if (path == 0) {
        return 0;
    }

    // 处理根目录
    if (str_compare(path, "/") == 0) {
        return g_root;
    }

    // 路径必须以 / 开头
    if (path[0] != '/') {
        return 0;
    }

    // 从根目录开始遍历
    current = g_root;
    start = 1;  // 跳过开头的 /

    while (path[start] != '\0') {
        // 查找下一个 /
        end = start;
        while (path[end] != '\0' && path[end] != '/') {
            end++;
        }

        // 提取路径组件
        if (end - start >= MAX_FILENAME_LEN) {
            return 0;  // 文件名过长
        }

        for (i = 0; i < end - start; i++) {
            component[i] = path[start + i];
        }
        component[end - start] = '\0';

        // 在当前目录中查找
        current = fs_find_in_directory(current, component);
        if (current == 0) {
            return 0;  // 路径不存在
        }

        // 移动到下一个组件
        if (path[end] == '/') {
            start = end + 1;
        } else {
            break;
        }
    }

    return current;
}

// ==================== 文件和目录操作函数 ====================

/**
 * 创建文件或目录
 */
FileSystemStatus fs_create(const char* path, const char* filename,
                          int owner_uid, int owner_gid,
                          Permissions perms, FileType type)
{
    FileNode* parent;
    FileNode* new_file;
    FileNode* last_child;

    // 参数验证
    if (path == 0 || filename == 0) {
        return FS_ERR_INVALID;
    }

    if (str_length(filename) == 0) {
        return FS_ERR_INVALID;
    }

    if (str_length(filename) >= MAX_FILENAME_LEN) {
        return FS_ERR_INVALID;
    }

    // 查找父目录
    parent = fs_find(path);
    if (parent == 0) {
        return FS_ERR_NOT_FOUND;
    }

    // 父路径必须是目录
    if (parent->type != FILE_TYPE_DIRECTORY) {
        return FS_ERR_NOT_DIR;
    }

    // 检查父目录的写权限
    if (fs_check_permission(parent, owner_uid, owner_gid, OP_WRITE) != PERM_GRANTED) {
        return FS_ERR_PERMISSION;
    }

    // 检查文件名是否已存在
    if (fs_find_in_directory(parent, filename) != 0) {
        return FS_ERR_EXISTS;
    }

    // 检查目录文件数量限制
    if (count_directory_children(parent) >= MAX_FILES_PER_DIR) {
        return FS_ERR_LIMIT;
    }

    // 分配新的文件节点
    new_file = allocate_file_node();
    if (new_file == 0) {
        return FS_ERR_LIMIT;
    }

    // 初始化文件节点
    str_copy(new_file->filename, filename, MAX_FILENAME_LEN);
    new_file->type = type;
    new_file->uid = owner_uid;
    new_file->gid = owner_gid;
    new_file->perms = perms;
    new_file->parent = parent;
    new_file->children = 0;
    new_file->next = 0;

    // 添加到父目录的子节点链表
    if (parent->children == 0) {
        parent->children = new_file;
    } else {
        // 找到链表末尾
        last_child = parent->children;
        while (last_child->next != 0) {
            last_child = last_child->next;
        }
        last_child->next = new_file;
    }

    return FS_SUCCESS;
}

/**
 * 删除文件或目录
 */
FileSystemStatus fs_delete(const char* path)
{
    FileNode* file;
    FileNode* parent;
    FileNode* prev;
    int uid, gid;

    // 参数验证
    if (path == 0) {
        return FS_ERR_INVALID;
    }

    // 不能删除根目录
    if (str_compare(path, "/") == 0) {
        return FS_ERR_INVALID;
    }

    // 查找文件
    file = fs_find(path);
    if (file == 0) {
        return FS_ERR_NOT_FOUND;
    }

    parent = file->parent;
    if (parent == 0) {
        return FS_ERR_INVALID;
    }

    // 获取当前用户
    uid = g_current_uid;
    gid = g_current_gid;

    // 检查父目录的写权限
    if (fs_check_permission(parent, uid, gid, OP_WRITE) != PERM_GRANTED) {
        return FS_ERR_PERMISSION;
    }

    // 如果是目录，检查是否为空
    if (file->type == FILE_TYPE_DIRECTORY) {
        if (file->children != 0) {
            return FS_ERR_NOT_EMPTY;
        }
    }

    // 从父目录的子节点链表中移除
    if (parent->children == file) {
        // 是第一个子节点
        parent->children = file->next;
    } else {
        // 查找前一个节点
        prev = parent->children;
        while (prev != 0 && prev->next != file) {
            prev = prev->next;
        }
        if (prev != 0) {
            prev->next = file->next;
        }
    }

    // 释放文件节点
    free_file_node(file);

    return FS_SUCCESS;
}

/**
 * 递归删除文件或目录
 * 使用后序遍历：先删除所有子节点，再删除节点本身
 */
FileSystemStatus fs_delete_recursive(const char* path)
{
    FileNode* file;
    FileNode* parent;
    FileNode* child;
    FileNode* next_child;
    FileNode* prev;
    int uid, gid;

    // 参数验证
    if (path == 0) {
        return FS_ERR_INVALID;
    }

    // 不能删除根目录
    if (str_compare(path, "/") == 0) {
        return FS_ERR_INVALID;
    }

    // 查找文件
    file = fs_find(path);
    if (file == 0) {
        return FS_ERR_NOT_FOUND;
    }

    parent = file->parent;
    if (parent == 0) {
        return FS_ERR_INVALID;
    }

    // 获取当前用户（这里简化处理，实际应该从上下文获取）
    // 暂时使用文件所有者的身份
    uid = file->uid;
    gid = file->gid;

    // 检查父目录的写权限
    if (fs_check_permission(parent, uid, gid, OP_WRITE) != PERM_GRANTED) {
        return FS_ERR_PERMISSION;
    }

    // 如果是目录，递归删除所有子节点
    if (file->type == FILE_TYPE_DIRECTORY) {
        child = file->children;
        while (child != 0) {
            next_child = child->next;

            // 构建子节点的完整路径以便递归删除
            // 这里我们直接操作节点，不需要构建完整路径
            // 递归删除子节点
            FileNode* child_parent = child->parent;
            FileNode* child_prev = 0;

            // 从父目录的子节点链表中找到要删除的子节点
            if (child_parent != 0) {
                if (child_parent->children == child) {
                    // 是第一个子节点
                    child_parent->children = child->next;
                } else {
                    // 查找前一个节点
                    child_prev = child_parent->children;
                    while (child_prev != 0 && child_prev->next != child) {
                        child_prev = child_prev->next;
                    }
                    if (child_prev != 0) {
                        child_prev->next = child->next;
                    }
                }
            }

            // 递归删除子节点（如果子节点是目录，会继续递归）
            // 先处理子节点的子节点（后序遍历）
            if (child->type == FILE_TYPE_DIRECTORY && child->children != 0) {
                // 需要构建路径递归调用，这里使用直接操作的方式
                // 暂时保存 next 指针
                FileNode* sub_child = child->children;
                while (sub_child != 0) {
                    FileNode* sub_next = sub_child->next;
                    // 从链表中移除
                    child->children = sub_child->next;
                    // 释放节点
                    free_file_node(sub_child);
                    sub_child = sub_next;
                }
            }

            // 释放子节点
            free_file_node(child);
            child = next_child;
        }
    }

    // 从父目录的子节点链表中移除当前节点
    if (parent->children == file) {
        // 是第一个子节点
        parent->children = file->next;
    } else {
        // 查找前一个节点
        prev = parent->children;
        while (prev != 0 && prev->next != file) {
            prev = prev->next;
        }
        if (prev != 0) {
            prev->next = file->next;
        }
    }

    // 释放文件节点
    free_file_node(file);

    return FS_SUCCESS;
}

/**
 * 修改文件权限
 */
FileSystemStatus fs_chmod(const char* path, Permissions perms)
{
    FileNode* file;
    int uid, gid;

    // 参数验证
    if (path == 0) {
        return FS_ERR_INVALID;
    }

    // 查找文件
    file = fs_find(path);
    if (file == 0) {
        return FS_ERR_NOT_FOUND;
    }

    // 获取当前用户
    uid = g_current_uid;
    gid = g_current_gid;

    // 只有所有者或 root 可以修改权限
    if (uid != file->uid && uid != 0) {
        return FS_ERR_PERMISSION;
    }

    // 修改权限
    file->perms = perms;

    return FS_SUCCESS;
}

/**
 * 修改文件所有者
 */
FileSystemStatus fs_chown(const char* path, int uid, int gid)
{
    FileNode* file;
    int current_uid;

    // 参数验证
    if (path == 0) {
        return FS_ERR_INVALID;
    }

    // 查找文件
    file = fs_find(path);
    if (file == 0) {
        return FS_ERR_NOT_FOUND;
    }

    // 获取当前用户
    current_uid = g_current_uid;

    // 只有 root 可以修改所有者
    if (current_uid != 0) {
        return FS_ERR_PERMISSION;
    }

    // 修改所有者
    file->uid = uid;

    // 如果 gid != -1，则修改组ID
    if (gid != -1) {
        file->gid = gid;
    }

    return FS_SUCCESS;
}

/**
 * 重命名文件或目录(在同一目录内)
 */
FileSystemStatus fs_rename(const char* path, const char* new_name, int uid, int gid)
{
    FileNode* file;
    FileNode* parent;
    char old_filename[MAX_FILENAME_LEN];

    // 参数验证
    if (path == 0 || new_name == 0) {
        return FS_ERR_INVALID;
    }

    if (str_length(new_name) == 0 || str_length(new_name) >= MAX_FILENAME_LEN) {
        return FS_ERR_INVALID;
    }

    // 不能重命名根目录
    if (str_compare(path, "/") == 0) {
        return FS_ERR_INVALID;
    }

    // 查找文件
    file = fs_find(path);
    if (file == 0) {
        return FS_ERR_NOT_FOUND;
    }

    parent = file->parent;
    if (parent == 0) {
        return FS_ERR_INVALID;
    }

    // 检查权限 - 需要对文件本身有写权限
    if (fs_check_permission(file, uid, gid, OP_WRITE) != PERM_GRANTED) {
        return FS_ERR_PERMISSION;
    }

    // 检查权限 - 需要对父目录有写权限
    if (fs_check_permission(parent, uid, gid, OP_WRITE) != PERM_GRANTED) {
        return FS_ERR_PERMISSION;
    }

    // 检查新文件名是否已存在
    if (fs_find_in_directory(parent, new_name) != 0) {
        return FS_ERR_EXISTS;
    }

    // 保存旧文件名用于调试
    str_copy(old_filename, file->filename, MAX_FILENAME_LEN);

    // 修改文件名
    str_copy(file->filename, new_name, MAX_FILENAME_LEN);

    return FS_SUCCESS;
}

/**
 * 检查目标路径是否是源路径的子目录
 * 用于防止将目录移动到其子目录中
 */
static int is_descendant(FileNode* src, FileNode* dest)
{
    FileNode* current;

    // 如果目标不是目录，则不可能是后代
    if (dest->type != FILE_TYPE_DIRECTORY) {
        return 0;
    }

    // 从目标目录向上遍历到根目录
    current = dest;
    while (current != 0) {
        if (current == src) {
            return 1;  // 找到了源节点，说明目标是源的后代
        }
        current = current->parent;
    }

    return 0;  // 不是后代
}

/**
 * 移动文件或目录到不同目录
 */
FileSystemStatus fs_move(const char* src_path, const char* dest_path, int uid, int gid)
{
    FileNode* src_file;
    FileNode* src_parent;
    FileNode* dest_parent;
    FileNode* existing_file;
    FileNode* prev;
    FileNode* last_child;
    char filename[MAX_FILENAME_LEN];
    // 参数验证
    if (src_path == 0 || dest_path == 0) {
        return FS_ERR_INVALID;
    }

    // 不能移动根目录
    if (str_compare(src_path, "/") == 0) {
        return FS_ERR_INVALID;
    }

    // 查找源文件
    src_file = fs_find(src_path);
    if (src_file == 0) {
        return FS_ERR_NOT_FOUND;
    }

    src_parent = src_file->parent;
    if (src_parent == 0) {
        return FS_ERR_INVALID;
    }

    // 查找目标目录
    dest_parent = fs_find(dest_path);
    if (dest_parent == 0) {
        return FS_ERR_NOT_FOUND;
    }

    // 目标必须是目录
    if (dest_parent->type != FILE_TYPE_DIRECTORY) {
        return FS_ERR_NOT_DIR;
    }

    // 检查是否将目录移动到其子目录中
    if (is_descendant(src_file, dest_parent)) {
        return FS_ERR_INVALID;
    }

    // 检查权限 - 需要对源文件有写权限
    if (fs_check_permission(src_file, uid, gid, OP_WRITE) != PERM_GRANTED) {
        return FS_ERR_PERMISSION;
    }

    // 检查权限 - 需要对源父目录有写权限
    if (fs_check_permission(src_parent, uid, gid, OP_WRITE) != PERM_GRANTED) {
        return FS_ERR_PERMISSION;
    }

    // 检查权限 - 需要对目标父目录有写权限
    if (fs_check_permission(dest_parent, uid, gid, OP_WRITE) != PERM_GRANTED) {
        return FS_ERR_PERMISSION;
    }

    // 保存源文件名
    str_copy(filename, src_file->filename, MAX_FILENAME_LEN);

    // 检查目标目录中是否已有同名文件
    existing_file = fs_find_in_directory(dest_parent, filename);
    if (existing_file != 0) {
        return FS_ERR_EXISTS;
    }

    // 从源父目录的子节点链表中移除
    if (src_parent->children == src_file) {
        // 是第一个子节点
        src_parent->children = src_file->next;
    } else {
        // 查找前一个节点
        prev = src_parent->children;
        while (prev != 0 && prev->next != src_file) {
            prev = prev->next;
        }
        if (prev != 0) {
            prev->next = src_file->next;
        }
    }

    // 更新父指针
    src_file->parent = dest_parent;

    // 添加到目标父目录的子节点链表
    src_file->next = 0;
    if (dest_parent->children == 0) {
        dest_parent->children = src_file;
    } else {
        // 找到链表末尾
        last_child = dest_parent->children;
        while (last_child->next != 0) {
            last_child = last_child->next;
        }
        last_child->next = src_file;
    }

    return FS_SUCCESS;
}

/**
 * 复制文件到指定目录
 */
FileSystemStatus fs_copy(const char* src_path, const char* dest_path, int uid, int gid)
{
    FileNode* src_file;
    FileNode* dest_parent;
    FileNode* new_file;
    FileNode* last_child;
    char filename[MAX_FILENAME_LEN];

    // 参数验证
    if (src_path == 0 || dest_path == 0) {
        return FS_ERR_INVALID;
    }

    // 查找源文件
    src_file = fs_find(src_path);
    if (src_file == 0) {
        return FS_ERR_NOT_FOUND;
    }

    // 只能复制普通文件，不能复制目录
    if (src_file->type != FILE_TYPE_REGULAR) {
        return FS_ERR_INVALID;
    }

    // 检查权限 - 需要对源文件有读权限
    if (fs_check_permission(src_file, uid, gid, OP_READ) != PERM_GRANTED) {
        return FS_ERR_PERMISSION;
    }

    // 查找目标目录
    dest_parent = fs_find(dest_path);
    if (dest_parent == 0) {
        return FS_ERR_NOT_FOUND;
    }

    // 目标必须是目录
    if (dest_parent->type != FILE_TYPE_DIRECTORY) {
        return FS_ERR_NOT_DIR;
    }

    // 检查权限 - 需要对目标父目录有写权限
    if (fs_check_permission(dest_parent, uid, gid, OP_WRITE) != PERM_GRANTED) {
        return FS_ERR_PERMISSION;
    }

    // 保存源文件名
    str_copy(filename, src_file->filename, MAX_FILENAME_LEN);

    // 检查目标目录中是否已有同名文件
    if (fs_find_in_directory(dest_parent, filename) != 0) {
        return FS_ERR_EXISTS;
    }

    // 检查目录文件数量限制
    if (count_directory_children(dest_parent) >= MAX_FILES_PER_DIR) {
        return FS_ERR_LIMIT;
    }

    // 分配新的文件节点
    new_file = allocate_file_node();
    if (new_file == 0) {
        return FS_ERR_LIMIT;
    }

    // 初始化新文件节点
    str_copy(new_file->filename, filename, MAX_FILENAME_LEN);
    new_file->type = src_file->type;           // 继承文件类型
    new_file->uid = uid;                        // 新文件由执行操作的用户所有
    new_file->gid = gid;                        // 使用执行操作的用户组
    new_file->perms = src_file->perms;          // 继承源文件的权限
    new_file->parent = dest_parent;
    new_file->children = 0;                     // 普通文件没有子节点
    new_file->next = 0;

    // 添加到目标父目录的子节点链表
    if (dest_parent->children == 0) {
        dest_parent->children = new_file;
    } else {
        // 找到链表末尾
        last_child = dest_parent->children;
        while (last_child->next != 0) {
            last_child = last_child->next;
        }
        last_child->next = new_file;
    }

    return FS_SUCCESS;
}
