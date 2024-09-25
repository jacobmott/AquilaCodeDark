import {
  ApplicationConfig,
  importProvidersFrom,
  provideZoneChangeDetection,
} from '@angular/core';

import {
  RouteReuseStrategy,
  provideRouter,
  RouterModule,
  Routes,
} from '@angular/router';
import {
  BaseRouteReuseStrategy,
  DefaultRouteReuseStrategy,
} from './route-strategy/base-route-reuse-strategy.service';
import { CustomRouteReuseStrategy } from './route-strategy/custom-route-reuse-strategy.service';

import { routes } from './app.routes';
import { provideHttpClient, withInterceptors } from '@angular/common/http';
import { environment } from '../environments/environment';

import { TopNavLightSliderService } from './shared/data-access/top-nav-light-slider.service';

export const appConfig: ApplicationConfig = {
  providers: [
    provideZoneChangeDetection({ eventCoalescing: true }),
    provideRouter(routes),
    provideHttpClient(),
    TopNavLightSliderService,
    {
      provide: RouteReuseStrategy,
      useClass: CustomRouteReuseStrategy,
    },
  ],
};
