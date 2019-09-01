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
example below. If a game does not need cpu affinity or environmental variables settings there is no need to modify its
launch command. Just start `fastgame_server` before launching the game.

# Features

- Change cpu frequency governor
- Change process niceness
- Change cpu affinity
- Set Environment variables like `__GL_THREADED_OPTIMIZATIONS` independently for each game
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
      "governor": {
        "default": "schedutil",
        "game": "performance"
      },
      "scheduler": {
        "sched_child_runs_first": {
          "default": "0",
          "game": "1"
        }
      }
    },
    "disk": {
      "device": "sdc",
      "scheduler": {
        "default": "mq-deadline",
        "game": "bfq"
      },
      "read-ahead": {
        "default": "256",
        "game": "1024"
      },
      "nr-requests": {
        "default": "64",
        "game": "1024"
      },
      "rq-affinity": {
        "default": "2",
        "game": "1"
      }
    },
    "nvidia": {
      "powermizer-mode": {
        "default": "adaptive",
        "game": "maximum-performance"
      },
      "power-limit": {
        "default": "180",
        "game": "200"
      },
      "clock-offset": {
        "gpu": {
          "default": "0",
          "game": "150"
        },
        "memory": {
          "default": "0",
          "game": "500"
        }
      }
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
    "RememberMe": {
      "environment": [
        "__GL_THREADED_OPTIMIZATIONS=0",
        "__GL_MaxFramesAllowed=2",
        "__GL_FSAA_MODE=11",
        "__GL_LOG_MAX_ANISO=4",
        "PROTON_USE_D9VK=1",
        "PROTON_FORCE_LARGE_ADDRESS_AWARE=1"
      ],
      "cpu-affinity": ["0", "1", "2", "3"],
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

When you install FastGame an empty configuration file will be created at `/etc/fastgame/config.json` and an exampple
will be copied to `/etc/fastgame/config.json`. Do not take the values there as good values for everybody. They are just
values I have been playing with in my computer. At least for now FastGame's main objective is giving the user the
ability to change system settings on demand. Customize your config file!

# Compilation

```
cd fastgame
meson build
cd build
ninja
```
