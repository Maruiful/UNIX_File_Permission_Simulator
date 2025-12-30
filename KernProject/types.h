#pragma once

// 最大限制常量
#define MAX_USERNAME_LEN    32
#define MAX_GROUPNAME_LEN   32
#define MAX_USERS           100
#define MAX_GROUPS          50
#define MAX_MEMBERS_PER_GROUP  100
#define MAX_FILENAME_LEN    64
#define MAX_PATH_LEN        256     // 最大路径长度
#define MAX_FILES           1000    // 系统中最大文件节点数量
#define MAX_FILES_PER_DIR   100     // 每个目录最大文件数
#define MAX_SUBDIRS_PER_DIR 50      // 每个目录最大子目录数

// 用户结构体
typedef struct {
    int uid;                            // 用户ID
    char username[MAX_USERNAME_LEN];    // 用户名
    int gid;                            // 所属主组ID
} User;

// 用户组结构体
typedef struct {
    int gid;                            // 组ID
    char groupname[MAX_GROUPNAME_LEN];  // 组名
    int member_uids[MAX_MEMBERS_PER_GROUP];  // 成员用户ID列表
    int member_count;                   // 成员数量
} Group;

// 文件类型枚举
typedef enum {
    FILE_TYPE_REGULAR,      // 普通文件
    FILE_TYPE_DIRECTORY     // 目录
} FileType;

// 权限位定义
#define PERM_READ       0x01    // 读权限 (r--)
#define PERM_WRITE      0x02    // 写权限 (-w-)
#define PERM_EXECUTE    0x04    // 执行权限 (--x)

// 权限掩码 - 9位权限: rwxrwxrwx
// 格式: [所有者权限][组权限][其他用户权限]
// 每组3位: 读(1) 写(2) 执行(4)

// 文件权限结构
typedef struct {
    unsigned char owner_perms;     // 所有者权限 (rwx)
    unsigned char group_perms;     // 组用户权限 (rwx)
    unsigned char other_perms;     // 其他用户权限 (rwx)
} Permissions;

// 文件节点结构体
typedef struct FileNode {
    char filename[MAX_FILENAME_LEN];   // 文件名
    FileType type;                     // 文件类型
    int uid;                           // 所有者用户ID
    int gid;                           // 所属组ID
    Permissions perms;                 // 权限
    struct FileNode* parent;           // 父目录指针
    struct FileNode* children;         // 子文件/目录链表(如果是目录)
    struct FileNode* next;             // 同级链表指针
    int inode;                         // inode编号(唯一标识)
} FileNode;

// 操作类型枚举
typedef enum {
    OP_READ,        // 读操作
    OP_WRITE,       // 写操作
    OP_EXECUTE,     // 执行操作
    OP_DELETE       // 删除操作
} OperationType;

// 权限检查结果
typedef enum {
    PERM_GRANTED,       // 权限授予
    PERM_DENIED,        // 权限拒绝
    PERM_ERROR          // 错误(无效参数等)
} PermissionResult;

// 用户管理状态码
typedef enum {
    USER_SUCCESS,           // 操作成功
    USER_ERR_EXISTS,        // 用户已存在
    USER_ERR_NOT_FOUND,     // 用户不存在
    USER_ERR_INVALID,       // 无效参数
    USER_ERR_LIMIT          // 达到最大限制
} UserStatus;

// 组管理状态码
typedef enum {
    GROUP_SUCCESS,          // 操作成功
    GROUP_ERR_EXISTS,       // 组已存在
    GROUP_ERR_NOT_FOUND,    // 组不存在
    GROUP_ERR_INVALID,      // 无效参数
    GROUP_ERR_LIMIT         // 达到最大限制
} GroupStatus;

// 文件系统状态码
typedef enum {
    FS_SUCCESS,             // 操作成功
    FS_ERR_EXISTS,          // 文件已存在
    FS_ERR_NOT_FOUND,       // 文件不存在
    FS_ERR_INVALID,         // 无效参数
    FS_ERR_PERMISSION,      // 权限不足
    FS_ERR_NOT_DIR,         // 不是目录
    FS_ERR_NOT_EMPTY,       // 目录非空
    FS_ERR_LIMIT            // 达到最大限制
} FileSystemStatus;
