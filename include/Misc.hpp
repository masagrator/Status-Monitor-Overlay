typedef struct {
    u8 ssid_len;                                         ///< NifmSfWirelessSettingData::ssid_len
    char ssid[0x21];                                     ///< NifmSfWirelessSettingData::ssid
    u8 unk_x22;                                          ///< NifmSfWirelessSettingData::unk_x21
    u8 pad;                                              ///< Padding
    u32 unk_x24;                                         ///< NifmSfWirelessSettingData::unk_x22
    u32 unk_x28;                                         ///< NifmSfWirelessSettingData::unk_x23
    u8 passphrase_len;                                   ///< Passphrase length
    u8 passphrase[0x41];                                 ///< NifmSfWirelessSettingData::passphrase
    u8 pad2[0x2];                                        ///< Padding
} NifmWirelessSettingData_new;

/// NetworkProfileData. Converted from/to \ref NifmSfNetworkProfileData.
typedef struct {
    Uuid uuid;                                           ///< NifmSfNetworkProfileData::uuid
    char network_name[0x40];                             ///< NifmSfNetworkProfileData::network_name
    u32 unk_x50;                                         ///< NifmSfNetworkProfileData::unk_x112
    u32 unk_x54;                                         ///< NifmSfNetworkProfileData::unk_x113
    u8 unk_x58;                                          ///< NifmSfNetworkProfileData::unk_x114
    u8 unk_x59;                                          ///< NifmSfNetworkProfileData::unk_x115
    u8 pad[2];                                           ///< Padding
    NifmWirelessSettingData_new wireless_setting_data;   ///< \ref NifmWirelessSettingData
    NifmIpSettingData ip_setting_data;                   ///< \ref NifmIpSettingData
} NifmNetworkProfileData_new;

Result getNvChannelClockRate(NvChannel *channel, u32 module_id, u32 *clock_rate) {
	struct nvhost_clk_rate_args {
	    uint32_t rate;
	    uint32_t moduleid;
	} args = {
		.rate     = 0,
		.moduleid = module_id,
	};

	u32 id = hosversionBefore(8,0,0) ? _NV_IOWR(0, 0x14, args) : _NV_IOWR(0, 0x23, args);
	Result rc = nvIoctl(channel->fd, id, &args);
	if (R_SUCCEEDED(rc) && clock_rate)
		*clock_rate = args.rate;

	return rc;
}