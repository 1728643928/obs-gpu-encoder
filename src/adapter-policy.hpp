// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <cstdint>
namespace gbe {
struct AdapterType {
    bool known=false;
    uint32_t flags=0;
};
// D3DKMT_ADAPTERTYPE bits from the Windows SDK.
// Preserve unknown devices and genuine same-model GPUs; never deduplicate by name.
constexpr bool encoding_candidate(bool dxgi_software,AdapterType type) {
    return !dxgi_software &&
           (!type.known ||
            (!(type.flags&(1u<<2)) && !((type.flags&(1u<<6)) && !(type.flags&1u))));
}
}
