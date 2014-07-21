#ifndef	__SYS_PARAM_H__
#define	__SYS_PARAM_H__

#define	CMD_LOGIN	76
#define CMD_DOWN	68
#define	CMD_UP		85
#define	CMD_LIST	83	
#define CMD_QUIT	81
#define	CMD_CMD		67

#define	STATE_LOGIN_CFM				0X00
#define	STATE_LOGIN_ERR				0X02

#define	STATE_UPFILE_REQ			0x52	//请求上传
#define	STATE_UPFILE_CFM			0x00	//确认请求
#define	STATE_UPFILE_END			0x03	//上传结束
#define	STATE_UPFILE_PRO			0x01	//正在上传
#define	STATE_UPFILE_EXT			0x06	//文件存在
#define	STATE_UPFILE_SPACE_FULL		0x08	//空间不足
#define	STATE_UPFILE_CRTFILE_ERR	0x0A	//创建文件失败

#define	STATE_DOWNFILE_REQ			0X52
#define	STATE_DOWNFILE_CFM			0X00
#define	STATE_DOWNFILE_PRO			0X02
#define	STATE_DOWNFILE_END			0X04
#define	STATE_DOWNFILE_NOEXIST		0X08
#define	STATE_DOWNFILE_SPACE_FULL	0X05
#define	STATE_DOWNFILE_CRTFILE_ERR	0X07

#define	STATE_FILELIST_ERR	0X04
#define	STATE_FILELIST_PRO	0X00
#define	STATE_FILELIST_END	0X02

#endif

