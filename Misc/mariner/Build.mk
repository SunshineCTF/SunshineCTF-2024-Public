# docker-compose challenge; `pwnmake check` starts it, runs the solver, and tears
# it down. k3s may take a while to create the flag secret, so allow extra time.
CHECK_TIMEOUT := 180
$(call ctf_check_tcp_slow,$(DIR),24501,bash solve.sh)
