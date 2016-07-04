#ifndef USERMANAGER_H
#define USERMANAGER_H

#define USER_TABLE_PATH "../userdata.txt"

struct user_info
{
	int status;
	char name[STR_MAX_LEN];
};

void init(struct user_info *usr)
{
	usr->status = -1;
	memset(usr->name, 0, sizeof(usr->name));
}

void setRecvUsername(struct user_info *usr, char *_name)
{
	usr->status = 0;
	memcpy(usr->name, _name, sizeof(_name));
}

void setLogin(struct user_info *usr)
{
	usr->status = 1;
}

int isLogin(struct user_info *usr)
{
	return usr->status == 1;
}

int isRecvUserName(struct user_info *usr)
{
	return strlen(usr->name) != 0;
}

int isReset(struct user_info *usr)
{
	return usr->status == -1;
}

int checkoutPassword(struct user_info *usr, char *password)
{
	freopen(USER_TABLE_PATH, "r", stdin);
	char line[STR_MAX_LEN], user[STR_MAX_LEN], pw[STR_MAX_LEN];
	while(gets(line))
	{
		dispatchCmd(line, user, pw);
		if(strcmp(user, usr->name) == 0 && strcmp(pw, password) == 0)
		{
			return 1;
		}
	}
	
	fclose(stdin);
	return 0;
}

#endif