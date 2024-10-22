#ifndef FLX_COMMUNICATIONS_NETWORK_SECRETS_HPP_
#define FLX_COMMUNICATIONS_NETWORK_SECRETS_HPP_

namespace network_secrets {

#ifdef FLXBOT_CONNECT_TEST_NETWORK
constexpr auto ssid = "Robottimo";
constexpr auto password = "slapstick-mold-sludge-blissful"; 
#else
constexpr auto ssid = "Robottimo";
constexpr auto password = "slapstick-mold-sludge-blissful"; 
#endif

}  // namespace network_secrets

#endif  // FLX_COMMUNICATIONS_NETWORK_SECRETS_HPP_

