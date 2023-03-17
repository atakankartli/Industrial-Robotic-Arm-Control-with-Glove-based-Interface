#!/bin/bash

interfaces='/etc/network/interfaces'
dhcpcd='/etc/dhcpcd.conf'


#######get ip via -i flag###### 

while getopts i: flag
do
	case "${flag}" in
		i) ip=${OPTARG};;
	esac
done

#split ip for subnet assignment

IFS=. read  var1 var2 var3 var4 <<< "${ip}"


#clear /etc/network/interfaces
sudo truncate -s 0 ${interfaces}

# /etc/network/interfaces dizinini değiştir
{
		printf "# A sample configuration for dhcpcd.\n# See dhcpcd.conf(5) for details.\n# Allow users of this group to interact with dhcpcd via the control socket."
		printf "\n#controlgroup wheel\n# Inform the DHCP server of our hostname for DDNS.\n"
		echo 'hostname'
		printf "# Use the hardware address of the interface for the Client ID.\n"
		echo 'clientid'
		printf "# or\n# Use the same DUID + IAID as set in DHCPv6 for DHCPv4 ClientID as per RFC4361.\n# Some non-RFC compliant DHCP servers do not reply with this set."
		printf "\n# Some non-RFC compliant DHCP servers do not reply with this set.\n# In this case, comment out duid and enable clientid above.\n#duid"
		printf "\n# Persist interface configuration when dhcpcd exits.\n"
		echo 'persistent'
		printf "# Rapid commit support.\n# Safe to enable by default because it requires the equivalent option set\n# on the server to actually work.\n"
		echo 'option rapid_commit'
		printf "# A list of options to request from the DHCP server.\n"
		echo 'option domain_name_servers, domain_name, domain_search, host_name'
		echo 'option classless_static_routes'
		printf "# Respect the network MTU. This is applied to DHCP routes.\n"
		echo 'option interface_mtu'
		printf "# Most distributions have NTP support.\n#option ntp_servers\n# A ServerID is required by RFC2131."
		printf "\n# Generate SLAAC address using the Hardware Address of the interface\n#slaac hwaddr\n# OR generate Stable Private IPv6 Addresses based from the DUID\n"
		echo 'slaac private'
		echo 'require dhcp_server_identifier'
		
		
		echo 'auto lo'
		echo 'iface lo inet loopback'
		echo 'auto eth0'
		echo 'iface eth0 inet static'
		echo '#your static IP'
		echo "address ${ip}"
		echo 'netmask 255.255.255.0'

}>> "${interfaces}"



# /etc/dhcpcd.conf dizinini değiştir
sudo truncate -s 0 ${dhcpcd}

{
	echo 'interface eth0'
	echo "static ip_address=${ip}/24"
	echo '#static ip6_address=fd51:42f8:caae:d92e::ff/64'
	echo "#static routers=${var1}.${var2}.${var3}.1"
	echo "static domain_name_servers=${var1}.${var2}.${var3}.1"
}>> "${dhcpcd}"



# ethernet down and up

sudo systemctl stop dhcpcd.service
sudo ip addr flush dev eth0
sudo systemctl start dhcpcd.service
#check if ip changed?