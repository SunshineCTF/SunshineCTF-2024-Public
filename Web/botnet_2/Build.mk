# Flag 2. Shares one docker-compose stack with Web/botnet_1; `pwnmake check`
# starts it, runs the solver, and tears it down. The solver listens for a
# callback from the challenge (via the docker bridge gateway). Needs
# python3-cryptography (repo-root prebuild.sh).
$(call ctf_check_web,$(DIR),24301,botnet.ctf.hackucf.org,python3 solver.py)

# botnet_1 and botnet_2 are one compose project, so they must not start or be
# checked at the same time under `-j`: start this stack after botnet_1's, and
# in a full `check`/`check-full` run, only after botnet_1's check (which tears
# the shared stack down) has finished.
BOTNET_1_DIR := $(patsubst %/botnet_2,%/botnet_1,$(DIR))
docker-start-one[$(DIR)]: | docker-start-one[$(BOTNET_1_DIR)]
ifneq ($(filter check check-full,$(MAKECMDGOALS)),)
docker-start-one[$(DIR)]: | check[$(BOTNET_1_DIR)]
endif
