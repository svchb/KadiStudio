message("\
                        ___
                     .-'   `'.
                    /         \\
                    |         ;
                    |         |           ___.--,
           _.._     |0) ~ (0) |    _.---'`__.-( (_.
    __.--'`_.. '.__.\\    '--. \\_.-' ,.--'`     `\"\"`
   ( ,.--'`   ',__ /./;   ;, '.__.'`    __
   _`) )  .---.__.' / |   |\\   \\__..--\"\"  \"\"\"--.,_
  `---' .'.''-._.-'`_./  /\\ '.  \\ _.-~~~````~~~-._`-.__.'
        | |  .' _.-' |  |  \\  \\  '.               `~---`
         \\ \\/ .'     \\  \\   '. '-._)
          \\/ /        \\  \\    `=.__`~-.
          / /\\         `) )    / / `\"\".`\\
    , _.-'.'\\ \\        / /    ( (     / /
     `--~`   ) )    .-'.'      '.'.  | (
            (/`    ( (`          ) )  '-;
             `      '-;         (-'
")

target_sources(kadistudio_framework PRIVATE
  pluginmanager.cpp
  plugindependencyloader.cpp
  plugins.cpp
  plugininfo/plugin.cpp
  plugininfo/plugininfo.cpp
  plugininfo/pluginstatus.cpp
  plugininfo/plugininterfacemacroreader.cpp
  plugininfo/pluginloader.cpp
  plugininfo/stringtokenizer.cpp
  plugininfo/pluginthread.cpp
  pluginchooser/pluginchooser.cpp
)
