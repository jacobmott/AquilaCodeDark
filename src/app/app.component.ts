import { Component, AfterViewInit } from '@angular/core';
import { RouterModule } from '@angular/router';
import { HttpClient } from '@angular/common/http';
import { SidebarComponent } from './shared/feature/sidebar/sidebar.component';
import { CommonModule } from '@angular/common';
import { EventBus } from '../game/EventBus';

@Component({
  standalone: true,
  imports: [RouterModule, SidebarComponent, CommonModule],
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrl: './app.component.css',
})
export class AppComponent implements AfterViewInit {
  isAuthenticated = false;
  isPhaserSceneReady = false;

  constructor(private http: HttpClient) {}

  ngAfterViewInit() {
    console.log('ngAfterViewInit AppComponent...');
    EventBus.on('current-scene-ready', (scene: Phaser.Scene) => {
      this.isPhaserSceneReady = true;
    });
  }

  isLoggedInAndPlaying() {
    return this.isAuthenticated && this.isPhaserSceneReady;
  }
}
