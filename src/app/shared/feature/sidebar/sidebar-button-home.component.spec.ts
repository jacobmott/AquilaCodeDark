import { ComponentFixture, TestBed } from "@angular/core/testing";
import { SidebarButtonHomeComponent } from "./sidebar-button-home.component";

describe("SidebarButtonHomeComponent", () => {
  let component: SidebarButtonHomeComponent;
  let fixture: ComponentFixture<SidebarButtonHomeComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [SidebarButtonHomeComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(SidebarButtonHomeComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it("should create", () => {
    expect(component).toBeTruthy();
  });
});
