  ####    ##  ##   ######    ####
   ##     ### ##   ##       ##  ##
   ##     ######   ##       ##  ##
   ##     ######   ####     ##  ##
   ##     ## ###   ##       ##  ##
   ##     ##  ##   ##       ##  ##
  ####    ##  ##   ##        ####

# Этот скрипт предназначен для размещения исполняемых файлов с соответствующих
# папках.

# Добавление скрипта в PlatformIO.
# Добавте опцию extra_scripts = firmware.py в настройки сборки проекта.

Import('env', 'projenv')

from pathlib import Path
from utils import get_firmware_version, options_get

print("Firmware: Start.")

 ##  ##   ######   #####     ####     ####     ####    ##  ##
 ##  ##   ##       ##  ##   ##  ##     ##     ##  ##   ### ##
 ##  ##   ##       ##  ##   ##         ##     ##  ##   ######
 ##  ##   ####     #####     ####      ##     ##  ##   ######
 ##  ##   ##       ####         ##     ##     ##  ##   ## ###
  ####    ##       ## ##    ##  ##     ##     ##  ##   ##  ##
   ##     ######   ##  ##    ####     ####     ####    ##  ##

# Эта секция формирует макросы VERSION_MAJOR, VERSION_MINOR.

# board_version.major - Эта опция указывает старший разряд версии проекта.
# Если данная опция добавлена в настройки проекта, то при компиляции проекта,
# будет добавлен макрос VERSION_MAJOR со значением указанном в этой опции.
# Этот параметр оказывает влияние на проект только если объявлен параметр
# board_version.minor.

# board_version.minor - Эта опция указывает старший разряд версии проекта.
# Если данная опция добавлена в настройки проекта, то при компиляции проекта,
# будет добавлен макрос VERSION_MINOR со значением указанном в этой опции.
# Этот параметр оказывает влияние на проект только если объявлен параметр
# board_version.major.

# Получаем версию файла.
firmware_version = get_firmware_version(env)

if firmware_version != None:
    firmware_version_major = firmware_version[0]
    firmware_version_minor = firmware_version[1]

    # Добавляем дефайны.
    # Добавляем именно в projenv т.к. если добавить в env то компилятор не видит эти дефайны.
    projenv.Append(CPPDEFINES=[
            ('VERSION_MAJOR', f'{firmware_version_major}'),
            ('VERSION_MINOR', f'{firmware_version_minor}')
        ]
    )
    print("Firmware: Defines VERSION_MAJOR and VERSION_MINOR were added.")
else:
    firmware_version_major = None
    firmware_version_minor = None
    print("Firmware: board_version not difened. Defines VERSION_MAJOR and VERSION_MINOR not added.")



####     ### ##            ### ###  ####       ##      ## ##    ## ##
 ##       ##  ##            ##  ##   ##         ##    ##   ##  ##   ##
 ##       ##  ##            ##       ##       ## ##   ##       ####
 ##       ##  ##            ## ##    ##       ##  ##  ##  ###   #####
 ##       ##  ##            ##       ##       ## ###  ##   ##      ###
 ##  ##   ##  ##            ##       ##  ##   ##  ##  ##   ##  ##   ##
### ###  ### ##            ####     ### ###  ###  ##   ## ##    ## ##

# Эта секция позволяет указать флаги для линкера.
# Эта секция добавлена т.к. встроенный функционал PlatformIO работал не корректно.

# board_ldflags - Этот параметр передаем флаги линкера. Если необходимо указать
# несколько флагов разделите их символом переноса строки.
try:
    # Получаем флаги для линкера.
    user_ldflags = env.GetProjectOption('board_ldflags').split()
except:
    pass
else:
    if len(user_ldflags) > 0:
        for flag in user_ldflags:
            env.Append(LINKFLAGS=[flag])
    print("Firmware: Flags from 'board_ldflags' were added.")


 #####    #####    ######   #####    ##  ##    ####    ##       ####
 ##  ##   ##  ##   ##       ##  ##   ##  ##     ##     ##       ## ##
 ##  ##   ##  ##   ##       ##  ##   ##  ##     ##     ##       ##  ##
 #####    #####    ####     #####    ##  ##     ##     ##       ##  ##
 ##       ####     ##       ##  ##   ##  ##     ##     ##       ##  ##
 ##       ## ##    ##       ##  ##   ##  ##     ##     ##       ## ##
 ##       ##  ##   ######   #####     ####     ####    ######   ####

# Эта секция реализует функцию предворительной компиляции указанных сред.

# board_firmware.prebuild - Этот параметр определяет какие среды будут
# перекомпелированы перед компеляцией проекта. Если необходимо куазать несколько
# сред разделите их символом переноса строки. Будьте осторожны скрипт
# не проверяет есть ли введенное имя в списке доступных сред.

try:
    # Получаем имена сред.
    pre_build_envs = env.GetProjectOption('board_firmware.prebuild').split()
except:
    pass
else:
    for item in pre_build_envs:
        if len(item) > 0:
            # Формируем действие.
            brebuild_action = env.VerboseAction(
                f"{env['PROJECT_CORE_DIR']}\\penv\\Scripts\\pio run -s -e {item}",
                f"Prebuild {item}..."
            )

            # Добавляем действие.
            env.AddPreAction(
                '$BUILD_DIR\\${PROGNAME}.elf',
                brebuild_action
            )

            print(f"Firmware: Prebuild action '{item}' was addes.")


 ##   ##  ######   ##   ##   ####    #####    ##  ##             ####     ####    ######   ######
 ### ###  ##       ### ###  ##  ##   ##  ##   ##  ##            ##  ##     ##         ##   ##
 #######  ##       #######  ##  ##   ##  ##   ##  ##            ##         ##        ##    ##
 ## # ##  ####     ## # ##  ##  ##   #####     ####              ####      ##       ##     ####
 ##   ##  ##       ##   ##  ##  ##   ####       ##                  ##     ##      ##      ##
 ##   ##  ##       ##   ##  ##  ##   ## ##      ##              ##  ##     ##     ##       ##
 ##   ##  ######   ##   ##   ####    ##  ##     ##               ####     ####    ######   ######

# Выводим объем памяти задействованый под программу.

# Формируем путь к исходному файлу.
source_file = '$BUILD_DIR\\${PROGNAME}.elf'

# Создаем действие.
print_memory_info = env.Action(
    f'arm-none-eabi-size {source_file}',
    f'Memory size'
)

# Добавляем действие.
env.AddPostAction(source_file, print_memory_info)


 ######    ####    #####    ##   ##  ##   ##    ##     #####    ######
 ##         ##     ##  ##   ### ###  ##   ##   ####    ##  ##   ##
 ##         ##     ##  ##   #######  ##   ##  ##  ##   ##  ##   ##
 ####       ##     #####    ## # ##  ## # ##  ######   #####    ####
 ##         ##     ####     ##   ##  #######  ##  ##   ####     ##
 ##         ##     ## ##    ##   ##  ### ###  ##  ##   ## ##    ##
 ##        ####    ##  ##   ##   ##  ##   ##  ##  ##   ##  ##   ######

# Эта секция переносит после компиляции исполняемые файлы в пользовательскую
# папку с указанным именем и добавляет версию если она указана в секции VERSION.

# board_firmware.name - Эта опция определяет имя исполняемых файлов.

# board_firmware.path - Эта опция указывает путь по которому должен быть помещен
# исполняемый файл. Путь должен включать относительный путь к папке
# и имя целевого файл разделенные косой чертой /.

# board_firmware.type - Этот параметр определяет типы файлов которые должны быть
# скопированы. Допустимые типы фпйлов bin, elf, srec. Если необходимо указать
# несколько типов, разделите каждый тип символом пробел или переноса строки.


# Все сделано через переменную skip потомучто нельзя прерывать скрипт с помощью
# функции exit() т.к. она прекращает выполнение остального скрипта.
skip = False

# Получаем имя окружения.
env_name = str(env['PIOENV'])

# Пропускаем генерацию исполняемых файлов если окружение не имеет в имени 
# 'factory' или 'firmware'.
if (env_name.find("factory") == -1) and \
   (env_name.find("firmware") == -1) and \
   (env_name.find("bootloader") == -1):
    skip = True
    print("Firmware: Skip firmware generation.")

# Получаем имя целевого файла.
if not skip:
    try:
        firmware_name = env.GetProjectOption('board_firmware.name')
    except:
        print("Firmware: board_firmware.name not difened. The firmware wiles will not be added.")
        skip = True

# Получаем путь целевого файла.
if not skip:
    try:
        firmware_path = Path(env.GetProjectOption('board_firmware.path')).joinpath(firmware_name)
    except:
        print("Firmware: board_firmware.path not difened. The firmware wiles will not be added.")
        skip = True

# Получаем типы файлов.
if not skip:
    try:
        firmware_types = env.GetProjectOption('board_firmware.type').split()
    except:
        print("Firmware: board_firmware.type not difened. The firmware wiles will not be added.")
        skip = True

# Проверяем введеные типы.
if not skip:
    if len(firmware_types) == 0:
        print("Firmware: board_firmware.type has not any type.")
        skip = True

# Проверяем имена типов.
if not skip:
    for firmware_type in firmware_types:
        if firmware_type != 'bin' and firmware_type != 'elf' and firmware_type != 'srec':
            print("Firmware: board_firmware.type has incorrect type.")
            skip = True

# Создаем действия для создания целевых файлов.
if not skip:
    for firmware_type in firmware_types:
        # Собираем имя исходного файла.
        source_file = '$BUILD_DIR\\${PROGNAME}.' + firmware_type

        # Собираем имя целевого файла.
        if firmware_version_major != None and firmware_version_minor != None:
            target_file = Path(f"{firmware_path}-{firmware_version_major}.{firmware_version_minor}.{firmware_type}").absolute()
        else:
            target_file = Path(f"{firmware_path}.{firmware_type}").absolute()

        # Находим все файлы с таким же типом.
        firmware_old_files = target_file.parent.glob(f"*.{firmware_type}")

        # Удаляем файлы прошивки.
        for firmware_old_file in firmware_old_files:

            # Создаем действие на удаление файла.
            delete_file = env.Action(
                # Заключаем все имена файлов в ковычки
                # так как консоль Windows не любит пробелы.
                f'del /Q /F "{str(firmware_old_file)}" > nul',
                f'Removing {str(firmware_old_file)}'
            )

            # Добавляем действие перед созданием целевого файла.
            env.AddPreAction(source_file, delete_file)

        # Копирование файла после создания.
        copy_file = env.Action(
            f'copy "{source_file}" "{target_file}" > nul',                          # '> nul' чтобы скрыть вывод.
            f'Copying {source_file} to {target_file}'
        )
        env.AddPostAction(source_file, copy_file)

        # Для файлов прошивки через CAN необходимо сгенерировать файл .srec отдельно.
        if firmware_type == 'srec':
            source_file = '$BUILD_DIR\\${PROGNAME}.elf'
            target_file = '$BUILD_DIR\\${PROGNAME}.' + firmware_type

            # Действие.
            # Определяет каким образом будет создаваться файл.
            act = env.Action(
                f'$OBJCOPY -O srec {source_file} {target_file}',                # Команда командной строки.
                f'Building {target_file}'                                       # Текст который будет выводиться в командную строку.
            )

            # Это решение работает, но только при нажатии кнопки "Build" при нажатии
            # кнопки "Upload" файл .srec не генерируется.
            # Билдер.
            # Создает одноразовый билдер.
            cmd = env.Command(target_file, source_file, act)

            # По умолчанию.
            # Добавляет в перечень целевых файлов, которые создаются по умолчанию.
            env.Default(cmd)

            # Добавляем действие которое должно выполниться после создания "source_file".
            env.AddPostAction(source_file, act)


 ######     ##      ####    ######    ####    #####    ##  ##
 ##        ####    ##  ##     ##     ##  ##   ##  ##   ##  ##
 ##       ##  ##   ##         ##     ##  ##   ##  ##   ##  ##
 ####     ######   ##         ##     ##  ##   #####     ####
 ##       ##  ##   ##         ##     ##  ##   ####       ##
 ##       ##  ##   ##  ##     ##     ##  ##   ## ##      ##
 ##       ##  ##    ####      ##      ####    ##  ##     ##

# В этой секции добавляются флаги компиляции для среды factory

# Добавляем уникальный флаг для сборки.
if env_name.find("factory") != -1:
    projenv.Append(CPPDEFINES=[
            ('BUILD_FACTORY', str(1))
        ]
    )
    print("Firmware: Define 'BUILD_FACTORY' was added.")


 #####    #####     ####      ####   ######    ####    ######    ####    ######     ##      ####    ##  ##    ####
 ##  ##   ##  ##   ##  ##      ##    ##       ##  ##     ##     ##  ##     ##      ####    ##  ##   ## ##    ##  ##
 ##  ##   ##  ##   ##  ##      ##    ##       ##         ##     ## ###     ##     ##  ##   ##       ####     ##
 #####    #####    ##  ##      ##    ####     ##         ##     ## ###     ##     ######    ####    ###       ####
 ##       ####     ##  ##      ##    ##       ##         ##     ##         ##     ##  ##       ##   ####         ##
 ##       ## ##    ##  ##   ## ##    ##       ##  ##     ##     ##   #     ##     ##  ##   ##  ##   ## ##    ##  ##
 ##       ##  ##    ####     ###     ######    ####      ##      ####      ##     ##  ##    ####    ##  ##    ####

# Эта секция добавляет комаду в дерево "PlatformIO/Project Tasks/" для всех ENV с суфиксом "factory"


# def before_upload(source, target, env):
#     print(f"before_upload {source[0]} ---------------")

# def before_build(source, target, env):
#     print(f"before_build {source[0]} ---------------")

# def before_checksize(source, target, env):
#     print(f"before_checksize {source[0]} ---------------")



def serialnumber_write(*args, **kwargs):
    print("===========================================")
    #print(env.Dump())
    print(env['PROJECT_CORE_DIR'])
    print("===========================================")

# def serialnumber_read(*args, **kwargs):
#     print("===========================================")
#     print("Reading Serial number")
#     print("===========================================")


# sn_tool_path = f"{env['PROJECT_DIR']}\\Tools\\TITAN1000-SerialNumber\\titan1000-serialnumber.exe"
# write_action = env.Action(f"{sn_tool_path} -w", None)
# read_action = env.Action(f"{sn_tool_path} -r", None)

#test_action = env.Action(serialnumber_write, None)

# if (str(env['PIOENV']).find("factory") != -1) and (env['UPLOAD_PROTOCOL'] == 'jlink') :
#     env.AddTarget(
#         name="write_serial_number",
#         group="Serial Number",
#         title="Write",
#         description="Write serial number to module",
#         dependencies=None,
#         #actions= env.Action(serialnumber_write, None),
#         actions=write_action
#     )

#     env.AddTarget(
#         name="read_serial_number",
#         group="Serial Number",
#         title="Read",
#         description="Read serial number from module",
#         dependencies=None,
#         #actions=env.Action(serialnumber_read, None),
#         actions=read_action
#     )

# env.AddCustomTarget(
#         name="testing",
#         title="Test123",
#         description="Test123",
#         dependencies=None,
#         #actions=env.Action(serialnumber_read, None),
#         actions=serialnumber_write
#     )


def release_action(*args, **kwargs):
    print("RELEASE")


env.AddTarget(
        name="release",
        group="General",
        title="Release",
        description="Release",
        dependencies=None,
        #actions=env.Action(serialnumber_read, None),
        actions=release_action
    )



#print(projenv.Dump())

print("Firmware: Finish.")