
#include "nsaecmd.h"
#include "nsaeipc.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int
main (void)
{
    int rc = 0;
    int exit_flag = 0;

    size_t packet_size = 0;
    nsae_packet_t *packet = NULL;

    const char *cmd_str = NULL;
    const char *var_str = NULL;
    const char *mode_str = NULL;


    nsae_ipc_init (NSAE_IPC_SERVER, NULL, NULL);

    while (!exit_flag)
    {
        /* wait for packet_size */
        packet_size = 0;
        rc = nsae_ipc_recieve (&packet_size, sizeof (packet_size));
        if (rc <= 0)
        {
            sleep (1);
            continue;
        }

        /* enforce minimum packet size */
        if (packet_size < sizeof (nsae_packet_t))
        {
            printf ("packet_size cannot be less than %zu -- %zu\n", 
                    sizeof (nsae_packet_t), packet_size);
            exit_flag = 1;
            continue;
        }

        /* wait for full packet */
        packet = realloc (packet, packet_size);
        rc = nsae_ipc_recieve_block (packet, packet_size);

        /* enforce that (sizeof (nsae_packet_t) + buflen) == packet_size */

        cmd_str = "";
        var_str = "";
        mode_str = "";

        switch (packet->cmd)
        {
            case CMD_QUIT:     cmd_str = "CMD_QUIT";     break;
            case CMD_RUN:      cmd_str = "CMD_RUN";      break;
            case CMD_STEP:     cmd_str = "CMD_STEP";     break;
            case CMD_NEXT:     cmd_str = "CMD_NEXT";     break;
            case CMD_INFO:     cmd_str = "CMD_INFO";     break;
            case CMD_SET:      cmd_str = "CMD_SET";      break;
            case CMD_DELETE:   cmd_str = "CMD_DELETE";   break;
            case CMD_LOAD:     cmd_str = "CMD_LOAD";     break;
            case CMD_SAVE:     cmd_str = "CMD_SAVE";     break;
            case CMD_READ:     cmd_str = "CMD_READ";     break;
            case CMD_WRITE:    cmd_str = "CMD_WRITE";    break;
            case CMD_NULL:     cmd_str = "CMD_NULL";     break;
            default:           cmd_str = "unknown";      break;
        }

        switch (packet->mode)
        {
            case MODE_NULL: mode_str = "MODE_NULL"; break;
            case MODE_ADV:  mode_str = "MODE_ADV";  break;
            case MODE_BR:   mode_str = "MODE_BR";   break;
            case MODE_CPU:  mode_str = "MODE_CPU";  break;
            case MODE_CRT:  mode_str = "MODE_CRT";  break;
            case MODE_FDC:  mode_str = "MODE_FDC";  break;
            case MODE_HDC:  mode_str = "MODE_HDC";  break;
            case MODE_KB:   mode_str = "MODE_KB";   break;
            case MODE_LOG:  mode_str = "MODE_LOG";  break;
            case MODE_MMU:  mode_str = "MODE_MMU";  break;
            case MODE_NSAE: mode_str = "MODE_NSAE"; break;
            case MODE_PROM: mode_str = "MODE_PROM"; break;
            case MODE_RAM:  mode_str = "MODE_RAM";  break;
            case MODE_ALL:  mode_str = "MODE_ALL";  break;
            default:        mode_str = "unknown";   break;
        }

        switch (packet->var)
        {
		case VAR_NULL:            var_str = "VAR_NULL";            break;
		case VAR_BR_APPEND:       var_str = "VAR_BR_APPEND";       break;

		case VAR_ADV_KBMI:        var_str = "VAR_ADV_KBMI";        break;
		case VAR_ADV_KBNMI:       var_str = "VAR_ADV_KBNMI";       break;
		case VAR_ADV_CRTMI:       var_str = "VAR_ADV_CRTMI";       break;
		case VAR_ADV_INTERUPT:    var_str = "VAR_ADV_INTERUPT";    break;
		case VAR_ADV_CMDACK:      var_str = "VAR_ADV_CMDACK";      break;

		case VAR_CPU_A:           var_str = "VAR_CPU_A";           break;
		case VAR_CPU_BC:          var_str = "VAR_CPU_BC";          break;
		case VAR_CPU_DE:          var_str = "VAR_CPU_DE";          break;
		case VAR_CPU_HL:          var_str = "VAR_CPU_HL";          break;
		case VAR_CPU_PC:          var_str = "VAR_CPU_PC";          break;
		case VAR_CPU_SP:          var_str = "VAR_CPU_SP";          break;
		case VAR_CPU_IX:          var_str = "VAR_CPU_IX";          break;
		case VAR_CPU_IY:          var_str = "VAR_CPU_IY";          break;
		case VAR_CPU_I:           var_str = "VAR_CPU_I";           break;
		case VAR_CPU_R:           var_str = "VAR_CPU_R";           break;
		case VAR_CPU_IFF1:        var_str = "VAR_CPU_IFF1";        break;
		case VAR_CPU_IFF2:        var_str = "VAR_CPU_IFF2";        break;
		case VAR_CPU_IM:          var_str = "VAR_CPU_IM";          break;
		case VAR_CPU_EXX:         var_str = "VAR_CPU_EXX";         break;
		case VAR_CPU_HALT:        var_str = "VAR_CPU_HALT";        break;
		case VAR_CPU_S_FLAG:      var_str = "VAR_CPU_S_FLAG";      break;
		case VAR_CPU_Z_FLAG:      var_str = "VAR_CPU_Z_FLAG";      break;
		case VAR_CPU_H_FLAG:      var_str = "VAR_CPU_H_FLAG";      break;
		case VAR_CPU_P_FLAG:      var_str = "VAR_CPU_P_FLAG";      break;
		case VAR_CPU_V_FLAG:      var_str = "VAR_CPU_V_FLAG";      break;
		case VAR_CPU_N_FLAG:      var_str = "VAR_CPU_N_FLAG";      break;
		case VAR_CPU_C_FLAG:      var_str = "VAR_CPU_C_FLAG";      break;

		case VAR_CRT_COLOR:       var_str = "VAR_CRT_COLOR";       break;
		case VAR_CRT_BLANK:       var_str = "VAR_CRT_BLANK";       break;
		case VAR_CRT_VSYNC:       var_str = "VAR_CRT_VSYNC";       break;
		case VAR_CRT_SCROLL:      var_str = "VAR_CRT_SCROLL";      break;

		case VAR_FDC_DISK:        var_str = "VAR_FDC_DISK";        break;
		case VAR_FDC_SIDE:        var_str = "VAR_FDC_SIDE";        break;
		case VAR_FDC_TRACK:       var_str = "VAR_FDC_TRACK";       break;
		case VAR_FDC_POWERED:     var_str = "VAR_FDC_POWERED";     break;
		case VAR_FDC_TRACKZERO:   var_str = "VAR_FDC_TRACKZERO";   break;
		case VAR_FDC_SECTORMARK:  var_str = "VAR_FDC_SECTORMARK";  break;
		case VAR_FDC_EJECT_A:     var_str = "VAR_FDC_EJECT_A";     break;
		case VAR_FDC_EJECT_B:     var_str = "VAR_FDC_EJECT_B";     break;
		case VAR_FDC_SECTOR_A:    var_str = "VAR_FDC_SECTOR_A";    break;
		case VAR_FDC_SECTOR_B:    var_str = "VAR_FDC_SECTOR_B";    break;

		case VAR_HDC_EJECT:       var_str = "VAR_HDC_EJECT";       break;

		case VAR_KB_REPEAT:       var_str = "VAR_KB_REPEAT";       break;
		case VAR_KB_CAPSLOCK:     var_str = "VAR_KB_CAPSLOCK";     break;
		case VAR_KB_CURSORLOCK:   var_str = "VAR_KB_CURSORLOCK";   break;
		case VAR_KB_OVERFLOW:     var_str = "VAR_KB_OVERFLOW";     break;
		case VAR_KB_DATAFLAG:     var_str = "VAR_KB_DATAFLAG";     break;
		case VAR_KB_INTERRUPT:    var_str = "VAR_KB_INTERRUPT";    break;
		case VAR_KB_PRESS:        var_str = "VAR_KB_PRESS";        break;

		case VAR_MMU_SLOT0:       var_str = "VAR_MMU_SLOT0";       break;
		case VAR_MMU_SLOT1:       var_str = "VAR_MMU_SLOT1";       break;
		case VAR_MMU_SLOT2:       var_str = "VAR_MMU_SLOT2";       break;
		case VAR_MMU_SLOT3:       var_str = "VAR_MMU_SLOT3";       break;

		case VAR_LOG_CRT:         var_str = "VAR_LOG_CRT";         break;
		case VAR_LOG_CPU:         var_str = "VAR_LOG_CPU";         break;
		case VAR_LOG_KB:          var_str = "VAR_LOG_KB";          break;
		case VAR_LOG_RAM:         var_str = "VAR_LOG_RAM";         break;
		case VAR_LOG_MMU:         var_str = "VAR_LOG_MMU";         break;
		case VAR_LOG_FDC:         var_str = "VAR_LOG_FDC";         break;
		case VAR_LOG_HDC:         var_str = "VAR_LOG_HDC";         break;
		case VAR_LOG_ADV:         var_str = "VAR_LOG_ADV";         break;
		case VAR_LOG_ALL:         var_str = "VAR_LOG_ALL";         break;
		case VAR_LOG_OUTPUT_FILE: var_str = "VAR_LOG_OUTPUT_FILE"; break;
        default:                  var_str = "unknown";             break;
        }

        printf ("{\n"
                "\t.cmd  = %02x   (%s),\n"
                "\t.var  =   %02x (%s),\n"
                "\t.mode = %04x (%s),\n"
                "\t.a = {\n"
                "\t\t.u32 = { %08x },\n"
                "\t\t.u16 = { %04x  %04x },\n"
                "\t\t.u8  = { %02x %02x %02x %02x }\n"
                "\t},\n"
                "\t.b = {\n"
                "\t\t.u32 = { %08x,\n"
                "\t\t.u16 = { %04x  %04x },\n"
                "\t\t.u8 =  { %02x %02x %02x %02x }\n"
                "\t},\n"
                "\t.buf = \"%.*s\"\n"
                "}\n",
                packet->cmd, cmd_str,
                packet->var, var_str,
                packet->mode, mode_str,
                packet->a.u32,
                packet->a.u16[0],
                packet->a.u16[1],
                packet->a.u8[0],
                packet->a.u8[1],
                packet->a.u8[2],
                packet->a.u8[3],
                packet->b.u32,
                packet->b.u16[0],
                packet->b.u16[1],
                packet->b.u8[0],
                packet->b.u8[1],
                packet->b.u8[2],
                packet->b.u8[3],
                (int)(packet_size - sizeof (*packet)),
                (char *)packet->buf);
    }

    free (packet);

    nsae_ipc_free (NSAE_IPC_SERVER);

    return 0;
}

/* end of file */
