#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// ==================== 文件系统管理函数 ====================

/**
 * 初始化文件系统
 * 在使用其他文件系统函数前必须调用
 * 创建根目录 "/"，所有者为 root (uid=0, gid=0)
 */
void fs_init(void);

// ==================== 文件和目录操作函数 ====================

/**
 * 创建文件或目录
 * @param path 父目录路径(如 "/home", "/home/user")
 * @param filename 文件名或目录名
 * @param owner_uid 所有者用户ID
 * @param owner_gid 所有者组ID
 * @param perms 权限设置
 * @param type 文件类型(FILE_TYPE_REGULAR 或 FILE_TYPE_DIRECTORY)
 * @return 操作状态码
 * @return FS_SUCCESS - 创建成功
 * @return FS_ERR_INVALID - 参数无效
 * @return FS_ERR_NOT_FOUND - 父目录不存在
 * @return FS_ERR_NOT_DIR - 父路径不是目录
 * @return FS_ERR_EXISTS - 文件名已存在
 * @return FS_ERR_PERMISSION - 权限不足
 * @return FS_ERR_LIMIT - 达到文件数量限制
 */
FileSystemStatus fs_create(const char* path, const char* filename,
                          int owner_uid, int owner_gid,
                          Permissions perms, FileType type);

/**
 * 删除文件或目录
 * @param path 完整路径(如 "/home/user/file.txt")
 * @return 操作状态码
 * @return FS_SUCCESS - 删除成功
 * @return FS_ERR_INVALID - 参数无效
 * @return FS_ERR_NOT_FOUND - 文件不存在
 * @return FS_ERR_PERMISSION - 权限不足
 * @return FS_ERR_NOT_EMPTY - 目录非空(不能删除非空目录)
 * @note 删除操作需要父目录的写权限
 * @note 只能删除空目录
 */
FileSystemStatus fs_delete(const char* path);

/**
 * 递归删除文件或目录
 * @param path 完整路径(如 "/home/user/dir")
 * @return 操作状态码
 * @return FS_SUCCESS - 删除成功
 * @return FS_ERR_INVALID - 参数无效
 * @return FS_ERR_NOT_FOUND - 文件不存在
 * @return FS_ERR_PERMISSION - 权限不足
 * @note 删除操作需要父目录的写权限
 * @note 如果是目录，会递归删除其所有内容
 * @note 不能删除根目录
 * @note 递归删除会检查每个子文件/目录的权限
 */
FileSystemStatus fs_delete_recursive(const char* path);

/**
 * 根据路径查找文件节点
 * @param path 完整路径(如 "/home/user/file.txt")
 * @return 文件节点指针，未找到返回NULL
 * @note 支持绝对路径，必须以 "/" 开头
 * @note 根目录路径为 "/"
 */
FileNode* fs_find(const char* path);

/**
 * 修改文件权限
 * @param path 文件路径
 * @param perms 新权限设置
 * @return 操作状态码
 * @return FS_SUCCESS - 修改成功
 * @return FS_ERR_INVALID - 参数无效
 * @return FS_ERR_NOT_FOUND - 文件不存在
 * @return FS_ERR_PERMISSION - 权限不足(只有所有者或root可以修改)
 */
FileSystemStatus fs_chmod(const char* path, Permissions perms);

/**
 * 修改文件所有者
 * @param path 文件路径
 * @param uid 新用户ID
 * @param gid 新组ID
 * @return 操作状态码
 * @return FS_SUCCESS - 修改成功
 * @return FS_ERR_INVALID - 参数无效
 * @return FS_ERR_NOT_FOUND - 文件不存在
 * @return FS_ERR_PERMISSION - 权限不足(只有root可以修改所有者)
 * @note 只有 root 用户可以修改文件所有者
 */
FileSystemStatus fs_chown(const char* path, int uid, int gid);

/**
 * 重命名文件或目录(在同一目录内)
 * @param path 文件完整路径
 * @param new_name 新文件名
 * @param uid 执行操作的用户ID
 * @param gid 执行操作的用户组ID
 * @return 操作状态码
 * @return FS_SUCCESS - 重命名成功
 * @return FS_ERR_INVALID - 参数无效
 * @return FS_ERR_NOT_FOUND - 文件不存在
 * @return FS_ERR_EXISTS - 新文件名已存在
 * @return FS_ERR_PERMISSION - 权限不足
 * @note 需要对文件本身有写权限
 * @note 需要对父目录有写权限
 */
FileSystemStatus fs_rename(const char* path, const char* new_name, int uid, int gid);

/**
 * 移动文件或目录到不同目录
 * @param src_path 源文件完整路径
 * @param dest_path 目标目录完整路径
 * @param uid 执行操作的用户ID
 * @param gid 执行操作的用户组ID
 * @return 操作状态码
 * @return FS_SUCCESS - 移动成功
 * @return FS_ERR_INVALID - 参数无效或移动到自己的子目录
 * @return FS_ERR_NOT_FOUND - 源文件或目标目录不存在
 * @return FS_ERR_EXISTS - 目标目录中已有同名文件
 * @return FS_ERR_NOT_DIR - 目标路径不是目录
 * @return FS_ERR_PERMISSION - 权限不足
 * @note 需要对源文件写权限
 * @note 需要对源父目录和目标父目录写权限
 * @note 不能将目录移动到其子目录中
 */
FileSystemStatus fs_move(const char* src_path, const char* dest_path, int uid, int gid);

/**
 * 复制文件到指定目录
 * @param src_path 源文件完整路径
 * @param dest_path 目标目录完整路径
 * @param uid 执行操作的用户ID
 * @param gid 执行操作的用户组ID
 * @return 操作状态码
 * @return FS_SUCCESS - 复制成功
 * @return FS_ERR_INVALID - 参数无效
 * @return FS_ERR_NOT_FOUND - 源文件或目标目录不存在
 * @return FS_ERR_EXISTS - 目标目录中已有同名文件
 * @return FS_ERR_NOT_DIR - 目标路径不是目录
 * @return FS_ERR_PERMISSION - 权限不足
 * @return FS_ERR_LIMIT - 达到文件数量限制
 * @note 需要对源文件有读权限
 * @note 需要对目标父目录有写权限
 * @note 只能复制普通文件，不能复制目录
 * @note 新文件由执行复制操作的用户所有
 * @note 新文件继承源文件的权限设置
 */
FileSystemStatus fs_copy(const char* src_path, const char* dest_path, int uid, int gid);

// ==================== 权限检查函数 ====================

/**
 * 检查用户对文件是否有执行某操作的权限
 * @param file 文件节点
 * @param uid 用户ID
 * @param gid 用户主组ID
 * @param op 操作类型(OP_READ/OP_WRITE/OP_EXECUTE/OP_DELETE)
 * @return 权限检查结果
 * @return PERM_GRANTED - 有权限
 * @return PERM_DENIED - 无权限
 * @return PERM_ERROR - 错误(无效参数)
 * @note root 用户(uid=0)自动拥有所有权限
 * @note 权限检查顺序:
 *   1. 如果 uid == file->uid，检查 owner_perms
 *   2. 否则如果用户属于 file->gid 组，检查 group_perms
 *   3. 否则检查 other_perms
 */
PermissionResult fs_check_permission(FileNode* file, int uid, int gid, OperationType op);

// ==================== 辅助函数 ====================

/**
 * 在目录中查找子文件
 * @param dir 父目录节点
 * @param filename 文件名
 * @return 找到返回文件节点指针，否则返回NULL
 * @note 遍历目录的 children 链表查找匹配的文件名
 * @note 如果 dir 不是目录，返回NULL
 */
FileNode* fs_find_in_directory(FileNode* dir, const char* filename);

/**
 * 路径解析辅助函数
 * @param path 完整路径(如 "/home/user/file.txt")
 * @param dirname 输出: 目录部分(如 "/home/user")
 * @param filename 输出: 文件名部分(如 "file.txt")
 * @return 成功返回0，失败返回-1
 * @note 对于根目录 "/"，dirname 和 filename 都为空
 * @note 调用者需确保 dirname 和 filename 缓冲区足够大
 */
int fs_parse_path(const char* path, char* dirname, char* filename);

/**
 * 规范化路径字符串
 * @param input 输入路径(可能包含 . 或 .. 或多个连续的 /)
 * @param output 输出缓冲区(至少 MAX_PATH_LEN 大小)
 * @return 成功返回0，失败返回-1
 * @note 处理 . (当前目录) 和 .. (父目录)
 * @note 合并连续的 /
 * @note 移除末尾的 / (根目录除外)
 * @note 支持相对路径和绝对路径
 * @note 示例: "/home/../user/./file.txt" -> "/user/file.txt"
 */
int fs_normalize_path(const char* input, char* output);

/**
 * 从指定目录开始查找相对路径
 * @param current_dir 当前目录节点
 * @param path 相对路径(不以 / 开头)或绝对路径
 * @return 找到的文件节点，未找到返回NULL
 * @note 支持 . (当前目录)
 * @note 支持 .. (父目录)
 * @note 不以 / 开头的路径视为相对路径
 * @note 以 / 开头的路径视为绝对路径，忽略 current_dir
 */
FileNode* fs_find_relative(FileNode* current_dir, const char* path);

/**
 * 获取文件数量统计
 * @return 当前系统中的文件节点总数
 */
int fs_get_file_count(void);

/**
 * 获取根目录节点
 * @return 根目录节点指针
 * @note 根目录在 fs_init() 时创建
 */
FileNode* fs_get_root(void);

#ifdef __cplusplus
}
#endif
