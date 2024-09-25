import { NgModule } from "@angular/core";
import { CommonModule } from "@angular/common";
// import { environment } from "../environments/environment";
import { SocketIoModule, SocketIoConfig } from "ngx-socket-io";
import { SocketAquila } from "./socket-aquila";
// import { AuthService } from "@auth0/auth0-angular";
// console.log(environment.socketUrl);
// const config: SocketIoConfig = {
//   url: environment.socketUrl,
//   options: {
//     path: environment.socketUrlPath,
//     query: {
//       token:
//         "",
//     },
//     // auth: {
//     //   token: "test",
//     // },
//     // extraHeaders: { Authorization: `Bearer test` },
//   },
// };

@NgModule({
  declarations: [],
  // imports: [CommonModule, SocketIoModule.forRoot(config)],
  imports: [CommonModule, SocketIoModule],
  providers: [],
})
export class SocketModule {}
