import { Injectable, NgModule } from "@angular/core";
import { Socket } from "ngx-socket-io";
import { environment } from "../environments/environment";
import { SocketIoConfig } from "ngx-socket-io";
import { AuthenticationService } from "./shared/data-access/authentication.service";

@Injectable()
export class SocketAquila extends Socket {
  constructor(private authenticationService: AuthenticationService) {
    super({ url: "", options: {} });

    const config: SocketIoConfig = {
      url: environment.socketUrl,
      options: {
        path: environment.socketUrlPath,
        query: {
          token: this.authenticationService.getToken(),
        },
        // auth: {
        //   token: "test",
        // },
        // extraHeaders: { Authorization: `Bearer test` },
      },
    };
    const any_this = this as any;
    any_this.config = config;
  }
}
