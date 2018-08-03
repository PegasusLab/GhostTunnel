#include "kismet_wifi_control.h"

#include "./interface_control.h"
#include "./capture_linux_wifi/linux_wireless_rfkill.h"
#include "./capture_linux_wifi/linux_wireless_control.h"
#include "./capture_linux_wifi/linux_netlink_control.h"


// kismet interface control
int kismet_interface_set_flags(const char *interface, char *errstr, int flags)
{
	return ifconfig_set_flags(interface, errstr, flags);
}

int kismet_interface_delta_flags(const char *interface, char *errstr, int flags)
{
	return ifconfig_delta_flags(interface, errstr, flags);
}

int kismet_interface_get_flags(const char *interface, char *errstr, int *flags)
{
	return ifconfig_get_flags(interface, errstr, flags);
}

int kismet_interface_up(const char *interface, char *errstr)
{
	return ifconfig_interface_up(interface, errstr);
}

int kismet_interface_down(const char *interface, char *errstr)
{
	return ifconfig_interface_down(interface, errstr);
}


// linux wireless rfkill
int kismet_get_rfkill(const char *interface, unsigned int rfkill_type)
{
	return linux_sys_get_rfkill(interface, rfkill_type);
}

int kismet_clear_rfkill(const char *interface)
{
	return linux_sys_clear_rfkill(interface);
}

// linux wireless control
int kismet_set_intpriv(const char *interface, const char *privcmd, int val1, int val2, char *errstr)
{
	return iwconfig_set_intpriv(interface, privcmd, val1, val2, errstr);
}

int kismet_get_intpriv(const char *interface, const char *privcmd, int *val, char *errstr)
{
	return iwconfig_get_intpriv(interface, privcmd, val, errstr);
}

int kismet_get_channel(const char *interface, char *errstr)
{
	return iwconfig_get_channel(interface, errstr);
}

int kismet_set_channel(const char *interface, int in_ch, char *errstr)
{
	return iwconfig_set_channel(interface, in_ch, errstr);
}

int kismet_get_mode(const char *interface, char *errstr, int *in_mode)
{
	return iwconfig_get_mode(interface, errstr, in_mode);
}

int kismet_set_mode(const char *interface, char *errstr, int in_mode)
{
	return iwconfig_set_mode(interface, errstr, in_mode);
}

/*int kismet_floatchan_to_int(float in_chan)
{
	return floatchan_to_int(in_chan);
}*/

int kismet_get_chanlist(const char *interface, char *errstr, unsigned int **chan_list, unsigned int *chan_list_len)
{
	return iwconfig_get_chanlist(interface, errstr, chan_list, chan_list_len);
}

int kismet_get_regdom(char *ret_countrycode)
{
	return linux_sys_get_regdom(ret_countrycode);
}

// linux netlink control
int kismet_nl80211_create_monitor_vif(const char *interface, const char *newinterface, unsigned int *flags, unsigned int flags_sz, char *errstr)
{
	return mac80211_create_monitor_vif(interface, newinterface, flags, flags_sz, errstr);
}

int kismet_nl80211_set_channel(const char *interface, int channel, unsigned int chmode, char *errstr)
{
	return mac80211_set_channel(interface, channel, chmode, errstr);
}

int kismet_nl80211_set_frequency(const char *interface, unsigned int control_freq, unsigned int chan_width, unsigned int center_freq1, unsigned int center_freq2, char *errstr)
{
	return mac80211_set_frequency(interface, control_freq, chan_width, center_freq1, center_freq2, errstr);
}

char *kismet_nl80211_find_parent(const char *interface)
{
	return mac80211_find_parent(interface);
}

int kismet_nl80211_get_chanlist(const char *interface, unsigned int extended_flags, char *errstr, char ***ret_chanlist, unsigned int *ret_chanlist_len)
{
	return mac80211_get_chanlist(interface, extended_flags, errstr, ret_chanlist, ret_chanlist_len);
}





