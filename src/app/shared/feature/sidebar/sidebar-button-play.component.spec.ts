import { ComponentFixture, TestBed } from "@angular/core/testing";
import { SidebarButtonPlayComponent } from "./sidebar-button-play.component";

describe("SidebarButtonShipsComponent", () => {
  let component: SidebarButtonPlayComponent;
  let fixture: ComponentFixture<SidebarButtonPlayComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [SidebarButtonPlayComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(SidebarButtonPlayComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it("should create", () => {
    expect(component).toBeTruthy();
  });
});
