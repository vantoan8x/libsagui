/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 * Cross-platform library which helps to develop web servers or frameworks.
 *
 * Copyright (C) 2016-2020 Silvio Clecio <silvioprog@gmail.com>
 *
 * Sagui library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Sagui library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sagui library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <stdint.h>
#ifdef _WIN32
#include <windows.h>
#else /* _WIN32 */
#include <unistd.h>
#endif /* _WIN32 */
#include <sagui.h>

/* NOTE: Error checking has been omitted to make it clear. */

#define CONNECTION_LIMIT 1000 /* Change to 10000 for C10K problem. */

static unsigned int get_cpu_count(void) {
#ifdef _WIN32
#ifndef _SC_NPROCESSORS_ONLN
  SYSTEM_INFO info;
  GetSystemInfo(&info);
#define sysconf(void) info.dwNumberOfProcessors
#define _SC_NPROCESSORS_ONLN
#endif /* _SC_NPROCESSORS_ONLN */
#endif /* _WIN32 */
#ifdef _SC_NPROCESSORS_ONLN
  return (unsigned int) sysconf(_SC_NPROCESSORS_ONLN);
#else /* _SC_NPROCESSORS_ONLN */
  return 0;
#endif /* _SC_NPROCESSORS_ONLN */
}

static void req_cb(__SG_UNUSED void *cls, __SG_UNUSED struct sg_httpreq *req,
                   struct sg_httpres *res) {
  sg_httpres_send(res, "Hello world", "text/plain", 200);
}

int main(int argc, const char *argv[]) {
  struct sg_httpsrv *srv;
  unsigned int cpu_count;
  uint16_t port;
  if (argc != 2) {
    printf("%s <PORT>\n", argv[0]);
    return EXIT_FAILURE;
  }
  port = strtol(argv[1], NULL, 10);
  cpu_count = get_cpu_count();
  srv = sg_httpsrv_new(req_cb, NULL);
  sg_httpsrv_set_thr_pool_size(srv, cpu_count);
  sg_httpsrv_set_con_limit(srv, CONNECTION_LIMIT);
  if (!sg_httpsrv_listen(srv, port, false)) {
    sg_httpsrv_free(srv);
    return EXIT_FAILURE;
  }
  fprintf(stdout, "Number of processors: %d\n", cpu_count);
  fprintf(stdout, "Connections limit: %d\n", CONNECTION_LIMIT);
  fprintf(stdout, "Server running at http://localhost:%d\n",
          sg_httpsrv_port(srv));
  fflush(stdout);
  getchar();
  sg_httpsrv_free(srv);
  return EXIT_SUCCESS;
}
