#!/usr/bin/bash

# remove .clangd/index if there is cached issues from clangd
# touch .nvim.lua
# touch .clangd

CLANGD_FILE=".clangd"
if [ -f "$CLANGD_FILE" ]; then
    echo "Warning: $CLANGD_FILE already exists."
    read -p "Overwrite? (y/n): " confirm
    if [[ $confirm != [yY] ]]; then
        echo "Aborting."
        exit 1
    fi
fi
cat <<'EOF' > $CLANGD_FILE
CompileFlags:
  Add: [
    "-target", "msp430",
    "-D__XC16__",
    "-D__PIC24FJ64GA002__",
    "-Dprog=",
    "-D__prog__=",
    "-D__pack__=",
    "-D__interrupt__=",
    "-D__section__(x)=",
    "-Wno-unknown-pragmas",
    "-I/opt/microchip/xc16/v2.10/support/PIC24F/h",
    "-I/opt/microchip/xc16/v2.10/support/generic/h"
  ]
  Remove: [-mcpu=*, -mdfp=*, -mresource=*, -msmart-io=*, -msfr-warn=*, -omf=*]

Diagnostics:
  Suppress: ["drv_unsupported_opt_for_target", "drv_unknown_argument", "unknown_typename"]
EOF
echo "Successfully created $CLANGD_FILE"

NVIM_FILE=".nvim.lua"
if [ -f "$NVIM_FILE" ]; then
	echo "Warning: $NVIM_FILE already exists."
    read -p "Overwrite? (y/n): " confirm
    if [[ $confirm != [yY] ]]; then
        echo "Aborting."
        exit 1
    fi
fi
cat <<'EOF' > "$NVIM_FILE"
local active_clients = vim.lsp.get_clients({ name = "clangd" })
for _, client in ipairs(active_clients) do
    if not vim.tbl_contains(client.config.cmd, "--query-driver=/opt/microchip/xc16/v2.10/bin/xc16-gcc") then
        vim.lsp.stop_client(client.id, true)
    end
end
require('lspconfig').clangd.setup({
  cmd = {
    "clangd",
    "--query-driver=/opt/microchip/xc16/v2.10/bin/xc16-gcc",
    "--background-index",
    "--header-insertion=never",
  },
})
EOF
echo "Successfully created $NVIM_FILE"

#Setup the clangd lsp via bear
make clean
export PATH="/opt/microchip/xc16/v2.10/bin:$PATH"
bear -- make -f nbproject/Makefile-default.mk SUBPROJECTS= .build-conf
sed -i 's/"xc16-gcc"/"\/opt\/microchip\/xc16\/v2.10\/bin\/xc16-gcc"/g' compile_commands.json
