# Fast Game

This is a project I started for fun and for learning purposes. It allows the user to apply small system configuration
tweaks that may increase gaming performance. The kernel Process Events Connector is used to monitor creation and
destruction of processes. When a process related to one of the games in our config file is detected the tweaks are
applied. Global tweaks like frequency governor changes are removed once no game process is running.

I start it manually before launching a game `sudo fastgame --config config.json`. At least for now no advantage in
keeping a service like this running when we are not playing.

# Features

- Change cpu frequency governor
- Change process niceness
- Change process cpu scheduler policy(SCHED_NORMAL, SCHED_BATCH,...)
- Change disk scheduler(bfq, mq-dealine,...), read ahead, nr_requests and rq_affinity values.
- Nvidia overclocking, power limit and powermize control

# Configuration File Example

```
{
  "general": {
    "cpu": {
      "default-governor": "schedutil",
      "game-governor": "performance"
    },
    "disk": {
      "device": "sdc",
      "default-scheduler": "mq-deadline",
      "game-scheduler": "bfq",
      "default-read-ahead": "256",
      "game-read-ahead": "2048",
      "default-nr-requests": "64",
      "game-nr-requests": "32",
      "default-rq-affinity": "2",
      "game-rq-affinity": "1"
    },
    "nvidia": {
      "default-powermizer-mode": "adaptive",
      "game-powermizer-mode": "maximum-performance",
      "default-gpu-clock-offset": "0",
      "game-gpu-clock-offset": "150",
      "default-memory-clock-offset": "0",
      "game-memory-clock-offset": "500",
      "default-power-limit": "180",
      "game-power-limit": "200"
    }
  },
  "games": {
    "SOTTR": {
      "niceness": "-4",
      "scheduler-policy": "SCHED_BATCH",
      "scheduler-policy-priority": "0",
      "io-class": "RT",
      "io-priority": "0"
    },
    "dota2": {
      "niceness": "-4",
      "scheduler-policy": "SCHED_BATCH",
      "scheduler-policy-priority": "0",
      "io-class": "RT",
      "io-priority": "0"
    },
    "APlagueTaleInnocence_x64": {
      "niceness": "-4",
      "scheduler-policy": "SCHED_BATCH",
      "scheduler-policy-priority": "0",
      "io-class": "RT",
      "io-priority": "0"
    }
  }
}

```

# Compilation

```
cd fastgame
meson build
cd build
ninja
```
