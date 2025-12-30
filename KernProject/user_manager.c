#include "user_manager.h"

// 用户存储数组
static User g_users[MAX_USERS];
static int g_user_count = 0;

// 用户组存储数组
static Group g_groups[MAX_GROUPS];
static int g_group_count = 0;

// ========== 用户管理实现 ==========

void um_init(void) {
    g_user_count = 0;
    g_group_count = 0;
}

UserStatus um_create_user(const char* username, int uid, int gid) {
    // 参数验证
    if (username == 0) {
        return USER_ERR_INVALID;
    }

    // 检查用户名长度
    int len = 0;
    const char* p = username;
    while (*p && len < MAX_USERNAME_LEN) {
        len++;
        p++;
    }
    if (len == 0 || len >= MAX_USERNAME_LEN) {
        return USER_ERR_INVALID;
    }

    // 检查是否达到最大用户数
    if (g_user_count >= MAX_USERS) {
        return USER_ERR_LIMIT;
    }

    // 检查UID是否已存在
    if (um_find_user_by_uid(uid) != 0) {
        return USER_ERR_EXISTS;
    }

    // 检查用户名是否已存在
    if (um_find_user_by_name(username) != 0) {
        return USER_ERR_EXISTS;
    }

    // 创建新用户
    User* user = &g_users[g_user_count];
    user->uid = uid;
    user->gid = gid;

    // 复制用户名
    p = username;
    char* dest = user->username;
    while (*p) {
        *dest = *p;
        dest++;
        p++;
    }
    *dest = '\0';

    g_user_count++;
    return USER_SUCCESS;
}

User* um_find_user_by_uid(int uid) {
    int i;
    for (i = 0; i < g_user_count; i++) {
        if (g_users[i].uid == uid) {
            return &g_users[i];
        }
    }
    return (User*)0;
}

User* um_find_user_by_name(const char* username) {
    int i;
    for (i = 0; i < g_user_count; i++) {
        // 比较用户名
        const char* p1 = g_users[i].username;
        const char* p2 = username;
        int match = 1;

        while (*p1 && *p2 && *p1 == *p2) {
            p1++;
            p2++;
        }

        if (*p1 == '\0' && *p2 == '\0') {
            return &g_users[i];
        }
    }
    return (User*)0;
}

int um_get_user_count(void) {
    return g_user_count;
}

UserStatus um_delete_user(int uid) {
    int i;
    int found = 0;

    // 查找用户位置
    for (i = 0; i < g_user_count; i++) {
        if (g_users[i].uid == uid) {
            found = 1;
            break;
        }
    }

    if (!found) {
        return USER_ERR_NOT_FOUND;
    }

    // 将后面的元素前移
    for (; i < g_user_count - 1; i++) {
        g_users[i] = g_users[i + 1];
    }

    g_user_count--;
    return USER_SUCCESS;
}

// ========== 用户组管理实现 ==========

GroupStatus um_create_group(const char* groupname, int gid) {
    // 参数验证
    if (groupname == 0) {
        return GROUP_ERR_INVALID;
    }

    // 检查组名长度
    int len = 0;
    const char* p = groupname;
    while (*p && len < MAX_GROUPNAME_LEN) {
        len++;
        p++;
    }
    if (len == 0 || len >= MAX_GROUPNAME_LEN) {
        return GROUP_ERR_INVALID;
    }

    // 检查是否达到最大组数
    if (g_group_count >= MAX_GROUPS) {
        return GROUP_ERR_LIMIT;
    }

    // 检查GID是否已存在
    if (um_find_group_by_gid(gid) != 0) {
        return GROUP_ERR_EXISTS;
    }

    // 检查组名是否已存在
    if (um_find_group_by_name(groupname) != 0) {
        return GROUP_ERR_EXISTS;
    }

    // 创建新组
    Group* group = &g_groups[g_group_count];
    group->gid = gid;
    group->member_count = 0;

    // 复制组名
    p = groupname;
    char* dest = group->groupname;
    while (*p) {
        *dest = *p;
        dest++;
        p++;
    }
    *dest = '\0';

    g_group_count++;
    return GROUP_SUCCESS;
}

Group* um_find_group_by_gid(int gid) {
    int i;
    for (i = 0; i < g_group_count; i++) {
        if (g_groups[i].gid == gid) {
            return &g_groups[i];
        }
    }
    return (Group*)0;
}

Group* um_find_group_by_name(const char* groupname) {
    int i;
    for (i = 0; i < g_group_count; i++) {
        // 比较组名
        const char* p1 = g_groups[i].groupname;
        const char* p2 = groupname;
        int match = 1;

        while (*p1 && *p2 && *p1 == *p2) {
            p1++;
            p2++;
        }

        if (*p1 == '\0' && *p2 == '\0') {
            return &g_groups[i];
        }
    }
    return (Group*)0;
}

GroupStatus um_add_group_member(int gid, int uid) {
    Group* group = um_find_group_by_gid(gid);
    if (group == 0) {
        return GROUP_ERR_NOT_FOUND;
    }

    // 检查用户是否存在
    if (um_find_user_by_uid(uid) == 0) {
        return GROUP_ERR_INVALID;
    }

    // 检查是否已经是成员
    int i;
    for (i = 0; i < group->member_count; i++) {
        if (group->member_uids[i] == uid) {
            return GROUP_ERR_EXISTS;  // 已经是成员
        }
    }

    // 检查是否达到最大成员数
    if (group->member_count >= MAX_MEMBERS_PER_GROUP) {
        return GROUP_ERR_LIMIT;
    }

    // 添加成员
    group->member_uids[group->member_count] = uid;
    group->member_count++;

    return GROUP_SUCCESS;
}

GroupStatus um_remove_group_member(int gid, int uid) {
    Group* group = um_find_group_by_gid(gid);
    if (group == 0) {
        return GROUP_ERR_NOT_FOUND;
    }

    // 查找成员位置
    int i;
    int found = 0;
    for (i = 0; i < group->member_count; i++) {
        if (group->member_uids[i] == uid) {
            found = 1;
            break;
        }
    }

    if (!found) {
        return GROUP_ERR_NOT_FOUND;
    }

    // 将后面的元素前移
    for (; i < group->member_count - 1; i++) {
        group->member_uids[i] = group->member_uids[i + 1];
    }

    group->member_count--;
    return GROUP_SUCCESS;
}

int um_is_group_member(int gid, int uid) {
    Group* group = um_find_group_by_gid(gid);
    if (group == 0) {
        return 0;
    }

    int i;
    for (i = 0; i < group->member_count; i++) {
        if (group->member_uids[i] == uid) {
            return 1;
        }
    }

    return 0;
}

int um_get_group_count(void) {
    return g_group_count;
}

GroupStatus um_delete_group(int gid) {
    int i;
    int found = 0;

    // 查找组位置
    for (i = 0; i < g_group_count; i++) {
        if (g_groups[i].gid == gid) {
            found = 1;
            break;
        }
    }

    if (!found) {
        return GROUP_ERR_NOT_FOUND;
    }

    // 将后面的元素前移
    for (; i < g_group_count - 1; i++) {
        g_groups[i] = g_groups[i + 1];
    }

    g_group_count--;
    return GROUP_SUCCESS;
}
