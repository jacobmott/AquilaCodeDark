import { Route } from '@angular/router';

export const routes: Route[] = [
  {
    path: '',
    redirectTo: 'home',
    pathMatch: 'full',
  }, // Redirect to '/home' when the path is empty
  {
    path: 'play',
    loadChildren: () =>
      import('./play/feature/play-shell/play-shell-routes').then(
        (routes) => routes.routes,
      ),
  },
  {
    path: 'home',
    loadChildren: () =>
      import('./home/feature/home-shell/home-shell-routes').then(
        (routes) => routes.routes,
      ),
  },
  // Add your another routes using this syntax.
  {
    path: 'ships',
    loadChildren: () =>
      import('./ships/feature/ship-shell/ship-shell-routes').then(
        (routes) => routes.routes,
      ),
  },
];
