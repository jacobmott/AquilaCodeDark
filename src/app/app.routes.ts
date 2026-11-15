import { Component } from '@angular/core';
import { Routes } from '@angular/router';
import { UniverseComponent } from './universe/universe.component';

// The parent owns the renderer. Child navigation changes the interface, not its lifetime.
@Component({ standalone: true, template: '' })
export class UniverseRouteComponent {}

export const routes: Routes = [
  {
    path: '',
    component: UniverseComponent,
    children: [
      { path: '', redirectTo: 'home', pathMatch: 'full' },
      ...['home', 'ships', 'atlas', 'play', 'settings'].map((path) => ({
        path,
        component: UniverseRouteComponent,
      })),
      { path: '**', redirectTo: 'home' },
    ],
  },
];
