# Fast Game

This is a project I started for fun and for learning purposes. It applies small performance tweaks that should be good
for games. The kernel Process Events Connector is used to monitor creation and destruction of processes. When a process
related to one of the games in our config file is detected the tweaks are applied. Global tweaks like frequency governor
changes are removed once no game process is running.

I start it manually before starting a game `sudo fastgame --config config.json`. There is no advantage in keeping a
service like this running when we are not playing.

# Features

- Change cpu frequency governor
- Change process niceness
- Change process cpu affinity and scheduler policy(SCHED_NORMAL, SCHED_BATCH,...)
- Where possible the tweaks can have different settings for each game. For example:

```
{
  "general": {
    "default-governor": "schedutil",
    "game-governor": "performance"
  },
  "games": {
    "Shadow of the Tomb Raider": {
      "niceness": "-4",
      "affinity-cores": ["0", "1", "2", "3", "4", "5", "6", "7"],
      "scheduler-policy": "SCHED_BATCH",
      "scheduler-policy-priority": "0",
      "io-class": "RT",
      "io-priority": "0"
    },
    "dota 2 beta": {
      "niceness": "-4",
      "scheduler-policy": "SCHED_BATCH",
      "scheduler-policy-priority": "0",
      "io-class": "RT",
      "io-priority": "0"
    },
    "A Plague Tale Innocence": {
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
