include Rules.make

.PHONY: basic_cgi basic_cgi_clean lan_cgi lan_cgi_clean  port_cgi port_cgi_clean encrypt_cgi encrypt_cgi_clean reboot_cgi reboot_cgi_clean system_cgi system_cgi_clean test_connect test_connect_clean \
top_cgi top_cgi_clean download_cgi download_cgi_clean get_info_cgi get_info_cgi_clean get_cpu_mem_cgi get_cpu_mem_cgi_clean pcap_ctrl_cgi pcap_ctrl_cgi_clean update_cgi update_cgi_clean \
upload_cgi upload_cgi_clean

default: all

common_interface:
	$(MAKE) -C src/CommonInterface
common_interface_clean:
	$(MAKE) -C src/CommonInterface clean

log_in_out_cgi:
	$(MAKE) -C src/LogInOutCgi
log_in_out_cgi_clean:
	$(MAKE) -C src/LogInOutCgi clean


basic_cgi:
	$(MAKE) -C src/BasicCgi
basic_cgi_clean:
	$(MAKE) -C src/BasicCgi clean

ifeq ($(strip $(BOX_TYPE)), $(filter $(BOX_TYPE), big_box_multiple_port big_box_encrypt big_box_elevator))
lan_cgi:
	$(MAKE) -C src/LanCgi_BigMulPort
lan_cgi_clean:
	$(MAKE) -C src/LanCgi_BigMulPort clean
endif


ifeq ($(strip $(BOX_TYPE)), $(filter $(BOX_TYPE), small_box big_box_single_port))
lan_cgi:
ifeq ($(BOX_TYPE_SUB), big_small_box_single_mulip_port)
	$(MAKE) -C src/LanCgi_SmallBigSigPort_mul_ip
else
	$(MAKE) -C src/LanCgi_SmallBigSigPort
endif
lan_cgi_clean:
ifeq ($(BOX_TYPE_SUB), big_small_box_single_mulip_port)
	$(MAKE) -C src/LanCgi_SmallBigSigPort_mul_ip clean
else
	$(MAKE) -C src/LanCgi_SmallBigSigPort clean
endif
endif

port_cgi:
	$(MAKE) -C src/PortCgi
port_cgi_clean:
	$(MAKE) -C src/PortCgi clean


ifeq ($(BOX_TYPE), big_box_encrypt)
encrypt_cgi:
	$(MAKE) -C src/EncryptCgi
encrypt_cgi_clean:
	$(MAKE) -C src/EncryptCgi clean
endif

reboot_cgi:
	@$(MAKE) -C src/RebootCgi
reboot_cgi_clean:
	@$(MAKE) -C src/RebootCgi clean

system_cgi:
	$(MAKE) -C src/SystemCgi
system_cgi_clean:
	$(MAKE) -C src/SystemCgi clean

download_cgi:
	$(MAKE) -C src/DownloadCgi
download_cgi_clean:
	$(MAKE) -C src/DownloadCgi clean

get_box_info_cgi:
	$(MAKE) -C src/GetBoxInfoCgi
get_box_info_cgi_clean:
	$(MAKE) -C src/GetBoxInfoCgi clean

update_cgi:
	$(MAKE) -C src/UpdateCgi
update_cgi_clean:
	$(MAKE) -C src/UpdateCgi clean

getqrcode_cgi:
	$(MAKE) -C src/GetQrcodeCgi
getqrcode_cgi_clean:
	$(MAKE) -C src/GetQrcodeCgi clean
	
restart_network_cgi:
	$(MAKE) -C src/RestartNetworkCgi
restart_network_cgi_clean:
	$(MAKE) -C src/RestartNetworkCgi clean

all:
	make common_interface
	make log_in_out_cgi
	make basic_cgi
	make lan_cgi
	make port_cgi
	make system_cgi
	make update_cgi
	make get_box_info_cgi
	make download_cgi
	make reboot_cgi
	make getqrcode_cgi
	make restart_network_cgi

clean:
	make common_interface_clean
	make log_in_out_cgi_clean
	make basic_cgi_clean
	make lan_cgi_clean
	make port_cgi_clean
	make system_cgi_clean
	make update_cgi_clean
	make get_box_info_cgi_clean
	make download_cgi_clean
	make reboot_cgi_clean
	make getqrcode_cgi_clean
	make restart_network_cgi_clean