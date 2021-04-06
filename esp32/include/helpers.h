#ifndef _ECHESS_HELPERS_H
#define _ECHESS_HELPERS_H

namespace echess {

template<class... Ts> struct visitor: Ts... { using Ts::operator()...; };
template<class... Ts> visitor(Ts...) -> visitor<Ts...>;

}

#endif
