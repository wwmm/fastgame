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
the game. On Steam you would do something like this `fastgame --game APlagueTaleInnocence_x64.exe --run %command%`. In
this case `APlagueTaleInnocence_x64.exe` is the name of `executable-name` field in the game profile file. There is a
profile example below. If a game does not need cpu affinity or environmental variables settings there is no need to
modify its launch command. Just start `fastgame_server` before launching the game.

# Features

- Change cpu frequency governor
- Change process niceness
- Change its cpu affinity
- Set Environment variables like `__GL_THREADED_OPTIMIZATIONS` independently for each game
- Change process cpu scheduler policy(SCHED_OTHER, SCHED_BATCH,...)
- Change disk scheduler(bfq, mq-dealine,...), read ahead, nr_requests and rq_affinity values
- Change io priority (as far as I know only bfq and cfq schedulers use this information)
- Nvidia overclocking, power limit and powermize control
- Radeon amdgpu power profile

# Configuration File Example

Settings that are independent of the game process should be put int the file `config.json`. It will look like this:

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
    "memory": {
      "hugepage": {
        "default": "madvise",
        "game": "always",
        "defrag": {
          "default": "madvise",
          "game": "always"
        },
        "shmem_enabled": {
          "default": "never",
          "game": "always"
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
        "default": "128",
        "game": "0"
      },
      "nr-requests": {
        "default": "64",
        "game": "32"
      },
      "rq-affinity": {
        "default": "2",
        "game": "1"
      }
    },
    "radeon": {
      "index": "1",
      "power_dpm_force_performance_level": {
        "default": "auto",
        "game": "high"
      }
    }
  }
}
```

Settings like cpu affinity can have different values for each game and must be put in our profiles folder. For example
the executable of the game A Plague Tale Innocence is named `APlagueTaleInnocence_x64.exe`. A profile for this game will
look like this

```
{
  "executable-name": "APlagueTaleInnocence_x64.exe",
  "environment": ["STAGING_SHARED_MEMORY=1", "WINEFSYNC_SPINCOUNT=2"],
  "niceness": "-4",
  "scheduler-policy": "SCHED_BATCH",
  "io-class": "RT",
  "io-priority": "0"
}
```

When you install FastGame an empty configuration file will be created at `/etc/fastgame/config.json` and an example
will be copied to `/etc/fastgame/config_example.json`. Profile examples can be found at `/etc/fastgame/profiles/examples`.
Put your profiels in the folder `/etc/fastgame/profiles/`. Do not take the values in the examples as good values for
everybody. They are just values I have been playing with in my computer. At least for now FastGame's main objective is
giving the user the ability to change system settings on demand. Customize your configuration files!

# Compilation

```
cd fastgame
meson build
cd build
ninja
```
