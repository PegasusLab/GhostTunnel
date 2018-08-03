#ifndef KISMET_WIFI_CONTROL_H
#define KISMET_WIFI_CONTROL_H

/*#ifdef __cpluscplus
extern "C"{
#endif*/

// kismet interface control
int kismet_interface_set_flags(const char *interface, char *errstr, int flags);
int kismet_interface_delta_flags(const char *interface, char *errstr, int flags);
int kismet_interface_get_flags(const char *interface, char *errstr, int *flags);
int kismet_interface_up(const char *interface, char *errstr);
int kismet_interface_down(const char *interface, char *errstr);

// kismet linux wireless rfkill
int kismet_get_rfkill(const char *interface, unsigned int rfkill_type);
int kismet_clear_rfkill(const char *interface);

// kismet linux wireless control
int kismet_set_intpriv(const char *interface, const char *privcmd, int val1, int val2, char *errstr);
int kismet_get_intpriv(const char *interface, const char *privcmd, int *val, char *errstr);
int kismet_get_channel(const char *interface, char *errstr);
int kismet_set_channel(const char *interface, int in_ch, char *errstr);
int kismet_get_mode(const char *interface, char *errstr, int *in_mode);
int kismet_set_mode(const char *interface, char *errstr, int in_mode);
//extern int kismet_floatchan_to_int(float in_chan);
int kismet_get_chanlist(const char *interface, char *errstr, unsigned int **chan_list, unsigned int *chan_list_len);
int kismet_get_regdom(char *ret_countrycode);

// kismet linux netlink control
int kismet_nl80211_create_monitor_vif(const char *interface, const char *newinterface, unsigned int *flags, unsigned int flags_sz, char *errstr);
int kismet_nl80211_set_channel(const char *interface, int channel, unsigned int chmode, char *errstr);
int kismet_nl80211_set_frequency(const char *interface, unsigned int control_freq, unsigned int chan_width, unsigned int center_freq1, unsigned int center_freq2, char *errstr);
char *kismet_nl80211_find_parent(const char *interface);
int kismet_nl80211_get_chanlist(const char *interface, unsigned int extended_flags, char *errstr, char ***ret_chanlist, unsigned int *ret_chanlist_len);

/*
#ifdef __cpluscplus
};
#endif*/


#endif 
