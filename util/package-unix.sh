#!/bin/bash
source $(dirname "$0")/bash-base.sh

arch=$1
package=$2
build_dir=${base_dir}/build/${arch}/unix
output_dir=${build_dir}/out

echo "[HUSARNET BS] Building unix ${arch} ${package} package"

fpm \
    --input-type dir \
    --output-type ${package} \
    --name husarnet \
    --version ${package_version} \
    --architecture ${arch} \
    --maintainer "Husarnet <support@husarnet.com>" \
    --vendor Husarnet \
    --description "Global LAN network" \
    --url "https://husarnet.com" \
    --depends iptables \
    --depends $(if [ "${package}" == "deb" ]; then echo "iproute2"; else echo "iproute"; fi) \
    --conflicts "husarnet-ros = 1.0.0" \
    --after-install ${base_dir}/platforms/unix/packaging/post-install-script.sh \
    --after-remove ${base_dir}/platforms/unix/packaging/post-remove-script.sh \
    --package ${build_dir}/husarnet-${arch}.${package} \
    --force \
    --chdir ${output_dir}

cp ${build_dir}/husarnet-${arch}.${package} ${release_base}/husarnet-${package_version}-${arch}.${package}
cp ${build_dir}/husarnet-${arch}.${package} ${release_base}/husarnet.${package}
