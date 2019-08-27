# FastGame

This is a project I started for fun and for learning purposes. It allows the user to apply small system configuration
tweaks that may increase gaming performance. The kernel Process Events Connector is used to monitor creation and
destruction of processes. When the kernel sends us the PID value of a recently created process we read the file
`/proc/PID/comm` and if the process name(without extension) matches one of the section names in the `games` section
of our config file the tweaks specified in this file are applied. Through its Process Events Connector the kernel also
notifies us about the PID value of finished processes. When one the the sent PID values matches the PID of the game
that was previously started FastGame will revert settings back to default values specfied in our config file.

You can start FastGame's service manually by executing `sudo fastgame_server`. On systems where FastGame was compiled
with Nvidia support the server has to be started after the login is done because Nvidia API needs to know the DISPLAY
value among other Xorg properties.

In other to set cpu affinity and environmental variables it is necessary to use the executable `fastgame` to launch
the game. On Steam you would do something like this `fastgame --game APlagueTaleInnocence_x64 --run %command%`. In this
case `APlagueTaleInnocence_x64` is the name of the game section in our `config.json` file. There is a configuration
example below. If a game does not need cpu affinity o environmental variables settings there is no need to modify its
launch command. Just start `fastgame_server` before launching the game.

# Features

- Change cpu frequency governor
- Change process niceness
- Change cpu affinity
- Set Environmental variables like `__GL_THREADED_OPTIMIZATIONS` independently for each game
- Change process cpu scheduler policy(SCHED_NORMAL, SCHED_BATCH,...)
- Change disk scheduler(bfq, mq-dealine,...), read ahead, nr_requests and rq_affinity values
- Change io priority (as far as I know only bfq and cfq schedulers use this information)
- Nvidia overclocking, power limit and powermize control

# Configuration File Example

A few settings can have different values for each game. When creating a new game section use the name of its executable
(without extension) as the section name. For example the executable of the game Shadow of the Tomb Raider is named
`SOTTR.exe`. When creating this game section name it `SOTTR`.

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
      "game-read-ahead": "1024",
      "default-nr-requests": "64",
      "game-nr-requests": "1024",
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
      "environment": [
        "__GL_THREADED_OPTIMIZATIONS=0",
        "__GL_MaxFramesAllowed=1"
      ],
      "cpu-affinity": ["0", "1", "2", "3", "4", "5", "6", "7"],
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
      "environment": [
        "__GL_THREADED_OPTIMIZATIONS=0",
        "__GL_MaxFramesAllowed=1"
      ],
      "cpu-affinity": ["0", "1", "2", "3", "4", "5", "6", "7"],
      "niceness": "-4",
      "scheduler-policy": "SCHED_BATCH",
      "scheduler-policy-priority": "0",
      "io-class": "RT",
      "io-priority": "0"
    }
  }
}

```

When you install FastGame a config file will be copied to `/etc/fastgame/config.json`. Do not take the values there
as good values for everybody. They are just values I have been playing with in my computer. At least for now FastGame's
main objective is giving the user the ability to change system settings on demand. Customize your config file!

# Compilation

```
cd fastgame
meson build
cd build
ninja
```
