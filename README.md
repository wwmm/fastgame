# Fast Game

This is a project I started for fun and for learning purposes. It applies small performance tweaks that should be good
for games. The kernel Process Events Connector is used to monitor creation and destruction of processes. When a process
related to one of the games in our config file is detected the tweaks are applied. Global tweaks like frequency governor
changes are removed once no game process is running.

I start it manually before launching a game `sudo fastgame --config config.json`. At least for now no advantage in
keeping a service like this running when we are not playing.

# Features

- Change cpu frequency governor
- Change process niceness
- Change process cpu affinity and scheduler policy(SCHED_NORMAL, SCHED_BATCH,...)
- Change disk scheduler(bfq, mq-dealine,...) and read ahead values.
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
      "default-scheduler": "bfq",
      "game-scheduler": "mq-deadline",
      "default-read-ahead": "256",
      "game-read-ahead": "2048"
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
      "affinity-cores": ["0", "1", "2", "3", "4", "5", "6", "7"],
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
      "affinity-cores": ["0", "1", "2", "3", "4", "5", "6", "7"],
      "scheduler-policy": "SCHED_BATCH",
      "scheduler-policy-priority": "0",
      "io-class": "RT",
      "io-priority": "0"
    }
  }
}

```
